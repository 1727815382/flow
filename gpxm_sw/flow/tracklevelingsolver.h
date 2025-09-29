#ifndef TRACK_LEVEL_SOLVER_H
#define TRACK_LEVEL_SOLVER_H
// TrackLevelingSolver.h ¡ª Qt 5.7 (C++11). Depends on Eigen (header-only).

#include <QtCore/QVector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <Eigen/Dense>

class TrackLevelingSolver
{
public:
    // ¡ª¡ª Measurement per mileage point ¡ª¡ª
    struct Measure {
        double s   = 0.0;  // mileage (m)
        double L   = 0.0;  // left rail elevation (mm)
        double C   = 0.0;  // centerline plan offset (mm), right positive
        double R   = 0.0;  // right rail elevation (mm)
        double Esup= 0.0;  // superelevation deviation (measured - design), optional
        double G   = 0.0;  // gauge deviation (measured - design), optional
    };

    // ¡ª¡ª Actuators (e.g., per row: Left vertical, Center horizontal, Right vertical) ¡ª¡ª
    enum class ActType { VertL, VertR, HorizCenter };
    struct Actuator {
        ActType type;
        double  s = 0.0;   // actuator mileage (m)
        // For vertical legs: horizontal leakage (mm horizontal per mm vertical)
        double  hLeak = 0.0;
        // For horizontal sliders: a => global translation (C), b => gauge change (G/2)
        double  a = 1.0, b = 0.0;
        int     id = -1;   // optional identifier
    };

    // ¡ª¡ª Physical / model parameters ¡ª¡ª
    struct Params {
        // Targeting options
        bool   keepMean   = false; // if true: preserve mean L/R while applying superelevation target
        double targetMean = 0.0;   // used when keepMean=false (e.g., return both rails to 0)

        // Superelevation control (R-L)
        // If you don't care about superelevation: set useSuperelevation=false (DEFAULT)
        bool   useSuperelevation   = false; // default off: drive both rails to targetMean
        bool   fixedSuperelevation = false; // if true, enforce S_target; else use Esup to infer design
        double S_target            = 0.0;   // desired superelevation (mm), e.g., 0

        // Coupling & spatial kernels
        double etaLR   = 0.20; // leftright vertical coupling
        double betaHV  = 0.00; // horizontal ¡ú vertical weak coupling
        double lambdaV = 2.0;  // vertical spatial influence length (m)
        double lambdaH = 2.0;  // horizontal spatial influence length (m)
        double gauge   = 1435.0; // gauge in mm (scaling only)

        // Regularization (Tikhonov + optional neighbor smoothness)
        double ridgeVert = 1e-4; // L2 penalty for vertical commands
        double ridgeHorz = 1e-4; // L2 penalty for horizontal commands
        double smoothV   = 0.0;  // neighbor smoothness for vertical actuators
        double smoothH   = 0.0;  // neighbor smoothness for horizontal actuators

        // Objective weighting and gauge inclusion
        bool   includeGauge = false;        // if true: also drive G¡ú0; else ignore G in objective
        double wL = 1.0, wR = 1.0, wC = 1.0, wG = 1.0; // weights for [L,R,C,G]

        // Safety tolerance for final residual (mm) when using planStep2
        double tol = 0.1;
    };

    struct FitSummary {
        Params theta;      // fitted parameters
        double rmseLRCG=0; // RMSE on [L,R,C,G] deltas during fit
    };

    // ¡ª¡ª ctor ¡ª¡ª (inline empty: header-only)
    TrackLevelingSolver();

    // ===== API =====

    // First pass: conservative command from current measurement (no history).
    QVector<double> planStep1(const QVector<Measure>& meas,
                              const QVector<Actuator>& acts,
                              Params p,
                              double firstScale = 0.7) const
    {
        auto M = sortByS(meas);
        auto A = sortActs(acts);

        Eigen::MatrixXd H; buildH(M, A, p, H);
        Eigen::VectorXd d = buildTargetDelta(M, p);
        Eigen::VectorXd x = solveCommands(H, d, A, p);
        x *= std::max(0.0, std::min(1.0, firstScale));
        return eigenToQVec(x);
    }

    // Fit parameters using previous measurement y0, next measurement y1 and the executed command step1Cmd.
    FitSummary fitParamsFromStep1(const QVector<Measure>& y0,
                                  const QVector<Measure>& y1,
                                  QVector<Actuator>& actsInOut,
                                  const QVector<double>& step1Cmd,
                                  const Params& seed) const
    {
        auto M0 = sortByS(y0);
        auto M1 = sortByS(y1);
        auto A  = sortActs(actsInOut);
        const int N  = M0.size();
        const int Mx = A.size();

        // ¦¤y vector [¦¤L, ¦¤R, ¦¤C, ¦¤G]
        Eigen::VectorXd dy(4*N); dy.setZero();
        for (int i=0;i<N;++i){
            dy(i)      = M1[i].L - M0[i].L;
            dy(N+i)    = M1[i].R - M0[i].R;
            dy(2*N+i)  = M1[i].C - M0[i].C;
            dy(3*N+i)  = M1[i].G - M0[i].G;
        }

        // First command vector
        Eigen::VectorXd x = qvecToEigen(step1Cmd, Mx);

        // Coarse grids
        const double ETA[] = {0.10,0.15,0.20,0.25,0.30,0.35};
        const double LAM[] = {1.0,1.5,2.0,3.0,4.0};
        const double BET[] = {0.00,0.05,0.10,0.15,0.20};

        FitSummary best; best.theta = seed; best.rmseLRCG = std::numeric_limits<double>::infinity();

        for (double eta : ETA)
        for (double lv  : LAM)
        for (double lh  : LAM)
        for (double beta: BET)
        {
            Params p = seed; p.etaLR=eta; p.lambdaV=lv; p.lambdaH=lh; p.betaHV=beta;

            // Fixed part: set a=b=hLeak=0 to isolate linear-in-coef columns
            auto A0 = A;
            for (auto& aa : A0) {
                if (aa.type==ActType::HorizCenter) { aa.a=0; aa.b=0; }
                else aa.hLeak=0;
            }
            Eigen::MatrixXd Hfix; buildH(M0, A0, p, Hfix);
            Eigen::VectorXd yfix = Hfix * x;

            // Design matrix Z for linear coefficients [all a's, all b's, all hLeak's]
            const int nA = countIf(A, ActType::HorizCenter);
            const int nV = Mx - nA;
            const int K  = 2*nA + nV;
            Eigen::MatrixXd Z = Eigen::MatrixXd::Zero(4*N, K);

            int col=0;
            // Columns for a (global translation contribution)
            for (int j=0;j<Mx;++j) if (A[j].type==ActType::HorizCenter){
                for (int i=0;i<N;++i){
                    const double sh = ker(M0[i].s - A[j].s, p.lambdaH);
                    const double u  = x(j);
                    Z(2*N+i, col) += sh * u; // C
                    const double dx = sh * u;
                    Z(i,     col) += -p.betaHV * 0.5 * dx; // L
                    Z(N+i,   col) += +p.betaHV * 0.5 * dx; // R
                }
                ++col;
            }
            // Columns for b (gauge change contribution)
            for (int j=0;j<Mx;++j) if (A[j].type==ActType::HorizCenter){
                for (int i=0;i<N;++i){
                    const double sh = ker(M0[i].s - A[j].s, p.lambdaH);
                    const double u  = x(j);
                    Z(3*N+i, col) += (2.0 * sh * u); // G
                    const double dxL = -sh * u, dxR = +sh * u;
                    Z(i,     col) += -p.betaHV * dxL; // L
                    Z(N+i,   col) += +p.betaHV * dxR; // R
                }
                ++col;
            }
            // Columns for hLeak (vertical actuators' horizontal leakage)
            for (int j=0;j<Mx;++j) if (A[j].type!=ActType::HorizCenter){
                for (int i=0;i<N;++i){
                    const double sh = ker(M0[i].s - A[j].s, p.lambdaH);
                    const double u  = x(j);
                    if (A[j].type==ActType::VertL){
                        Z(2*N+i, col) += 0.5 * sh * u;  // C
                        Z(3*N+i, col) += -1.0 * sh * u; // G
                        Z(i,     col) += -p.betaHV * sh * u; // L via betaHV
                    } else {
                        Z(2*N+i, col) += 0.5 * sh * u;
                        Z(3*N+i, col) += +1.0 * sh * u;
                        Z(N+i,   col) += +p.betaHV * sh * u; // R via betaHV
                    }
                }
                ++col;
            }

            // Regress (dy - fixed) on Z to obtain a,b,hLeak
            Eigen::VectorXd coef = Z.colPivHouseholderQr().solve(dy - yfix);

            // Apply new gains
            col=0;
            for (auto& aa : A) if (aa.type==ActType::HorizCenter) aa.a     = coef(col++);
            for (auto& aa : A) if (aa.type==ActType::HorizCenter) aa.b     = coef(col++);
            for (auto& aa : A) if (aa.type!=ActType::HorizCenter) aa.hLeak = coef(col++);

            // Evaluate RMSE with updated gains
            Eigen::MatrixXd H; buildH(M0, A, p, H);
            const double rmse = std::sqrt(((H*x - dy).squaredNorm()) / (4.0*N));
            if (rmse < best.rmseLRCG) { best.rmseLRCG = rmse; best.theta = p; actsInOut = A; }
        }
        return best;
    }

    // Second pass: one-shot command from current measurement with safety scaling (not to overshoot)
    QVector<double> planStep2(const QVector<Measure>& meas,
                              const QVector<Actuator>& tunedActs,
                              const Params& tunedParams,
                              double* outPredMaxAbsResidual = nullptr) const
    {
        auto M1 = sortByS(meas);
        auto A  = sortActs(tunedActs);

        Eigen::MatrixXd H; buildH(M1, A, tunedParams, H);
        Eigen::VectorXd d  = buildTargetDelta(M1, tunedParams);
        Eigen::VectorXd x  = solveCommands(H, d, A, tunedParams);

        // scalar line search k¡Ê(0,1] so that ||d - k*(H*x)||_¡Þ ¡Ü tol
        Eigen::VectorXd Hx = H * x;
        double lo = 0.0, hi = 1.0;
        for (int it=0; it<25; ++it) {
            double mid = 0.5*(lo+hi);
            double maxabs = (d - mid*Hx).cwiseAbs().maxCoeff();
            if (maxabs <= tunedParams.tol) hi = mid; else lo = mid;
        }
        x *= hi; // scale commands

        if (outPredMaxAbsResidual){
            // After scaling, predicted residual = d - H*(x)
            *outPredMaxAbsResidual = (d - H*(x)).cwiseAbs().maxCoeff();
        }
        return eigenToQVec(x);
    }

private:
    // ===== Internals =====
    static inline double ker(double ds, double lambda_m)
    {
        ds = std::abs(ds);
        return (lambda_m<=1e-6) ? (ds<1e-9?1.0:0.0) : std::exp(-ds/lambda_m);
    }

    static QVector<Measure> sortByS(QVector<Measure> v)
    {
        std::sort(v.begin(), v.end(), [](const Measure&a, const Measure&b){ return a.s < b.s; });
        return v;
    }
    static QVector<Actuator> sortActs(QVector<Actuator> v)
    {
        std::sort(v.begin(), v.end(), [](const Actuator&a, const Actuator&b){ return a.s < b.s; });
        return v;
    }
    static int countIf(const QVector<Actuator>& A, ActType t)
    {
        int c=0; for (auto& a:A) if (a.type==t) ++c; return c;
    }

    // Build forward matrix H: commands x ¡ú ¦¤y=[¦¤L,¦¤R,¦¤C,¦¤G]
    static void buildH(const QVector<Measure>& M,
                       const QVector<Actuator>& A,
                       const Params& P,
                       Eigen::MatrixXd& H)
    {
        const int N = M.size();
        const int MX = A.size();
        const int Y = 4*N; // rows: L,R,C,G
        H.setZero(Y, MX);

        auto rL=[&](int i){return i;};
        auto rR=[&](int i){return N+i;};
        auto rC=[&](int i){return 2*N+i;};
        auto rG=[&](int i){return 3*N+i;};

        for (int i=0;i<N;++i){
            for (int j=0;j<MX;++j){
                const double sv = ker(M[i].s - A[j].s, P.lambdaV);
                const double sh = ker(M[i].s - A[j].s, P.lambdaH);

                if (A[j].type==ActType::VertL){
                    // Vertical: left ¡ú L, coupled to R by etaLR
                    H(rL(i), j) += sv * 1.0;
                    H(rR(i), j) += sv * P.etaLR;
                    // Horizontal leakage from vertical motion
                    const double dxL = sh * A[j].hLeak;
                    H(rC(i), j) += 0.5 * dxL;   // C = (xR+xL)/2
                    H(rG(i), j) += -1.0 * dxL;  // G = xR - xL
                    // Horizontal¡úVertical weak coupling
                    H(rL(i), j) += -P.betaHV * dxL;
                }
                else if (A[j].type==ActType::VertR){
                    H(rL(i), j) += sv * P.etaLR;
                    H(rR(i), j) += sv * 1.0;
                    const double dxR = sh * A[j].hLeak;
                    H(rC(i), j) += 0.5 * dxR;
                    H(rG(i), j) += +1.0 * dxR;
                    H(rR(i), j) += +P.betaHV * dxR;
                }
                else{ // HorizCenter
                    // one command ¡ú translation a¡¤u (C) and gauge 2b¡¤u (G)
                    const double tC = sh * A[j].a;
                    const double tG = sh * 2.0 * A[j].b;
                    H(rC(i), j) += tC;
                    H(rG(i), j) += tG;
                    // map to left/right horizontal shifts
                    const double dxL = sh * (A[j].a - A[j].b);
                    const double dxR = sh * (A[j].a + A[j].b);
                    // Horizontal¡úVertical weak coupling
                    H(rL(i), j) += -P.betaHV * dxL;
                    H(rR(i), j) += +P.betaHV * dxR;
                }
            }
        }
    }

    // Build target vector d=[dL,dR,dC,dG]
    static Eigen::VectorXd buildTargetDelta(const QVector<Measure>& M, const Params& P)
    {
        const int N = M.size();
        Eigen::VectorXd d(4*N); d.setZero();
        for (int i=0;i<N;++i){
            const double L=M[i].L, R=M[i].R, C=M[i].C, G=M[i].G;

            double dL = 0.0, dR = 0.0;
            if (P.useSuperelevation) {
                // enforce superelevation: either fixed S_target or restored design via Esup
                const double Sdes = P.fixedSuperelevation ? P.S_target : ((R - L) - M[i].Esup);
                const double m    = P.keepMean ? 0.5*(L+R) : P.targetMean;
                dL = (m - 0.5*Sdes) - L;
                dR = (m + 0.5*Sdes) - R;
            } else {
                // ignore superelevation: drive both rails to targetMean (usually 0)
                dL = (P.targetMean) - L;
                dR = (P.targetMean) - R;
            }

            d(i)      = P.wL * dL;        // L
            d(N+i)    = P.wR * dR;        // R
            d(2*N+i)  = P.wC * (-C);      // C back to 0
            d(3*N+i)  = P.includeGauge ? (P.wG * (-G)) : 0.0; // G optional
        }
        return d;
    }

    // Solve (H^T H + R) x = H^T d with Tikhonov + optional neighbor smoothing
    static Eigen::VectorXd solveCommands(const Eigen::MatrixXd& H,
                                         const Eigen::VectorXd& d,
                                         const QVector<Actuator>& A,
                                         const Params& P)
    {
        const int MX = A.size();
        Eigen::MatrixXd R = Eigen::MatrixXd::Zero(MX, MX);
        for (int j=0;j<MX;++j){
            if (A[j].type==ActType::HorizCenter) R(j,j) = P.ridgeHorz;
            else                                  R(j,j) = P.ridgeVert;
        }
        // Optional neighbor smoothing (simple chain by mileage)
        if (P.smoothV>0.0 || P.smoothH>0.0){
            int prevV=-1, prevH=-1;
            for (int j=0;j<MX;++j){
                if (A[j].type==ActType::HorizCenter){
                    if (prevH!=-1){ R(prevH,prevH)+=P.smoothH; R(j,j)+=P.smoothH; R(prevH,j)-=P.smoothH; R(j,prevH)-=P.smoothH; }
                    prevH=j;
                } else {
                    if (prevV!=-1){ R(prevV,prevV)+=P.smoothV; R(j,j)+=P.smoothV; R(prevV,j)-=P.smoothV; R(j,prevV)-=P.smoothV; }
                    prevV=j;
                }
            }
        }
        Eigen::MatrixXd N = H.transpose()*H + R;
        Eigen::VectorXd g = H.transpose()*d;
        return N.colPivHouseholderQr().solve(g);
    }

    // QtEigen helpers
    static QVector<double> eigenToQVec(const Eigen::VectorXd& v){
        QVector<double> out; out.resize(int(v.size()));
        for (int i=0;i<v.size();++i) out[i]=v(i);
        return out;
    }
    static Eigen::VectorXd qvecToEigen(const QVector<double>& v, int expect=-1){
        Eigen::VectorXd out(v.size()); for (int i=0;i<v.size();++i) out(i)=v[i];
        if (expect>=0 && expect!=v.size()){
            Eigen::VectorXd fix = Eigen::VectorXd::Zero(expect);
            const int n = std::min(expect, int(v.size()));
            for (int i=0;i<n;++i) fix(i)=v[i];
            return fix;
        }
        return out;
    }
};

#endif // TRACK_LEVEL_SOLVER_H
