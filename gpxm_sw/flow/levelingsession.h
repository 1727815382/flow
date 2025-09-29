#ifndef LEVELINGSESSION_H
#define LEVELINGSESSION_H

#include "TrackLevelingSolver.h"
#include <QVector>

// Iterative session: measure ¡ú compute command ¡ú execute ¡ú re-measure ¡ú compute next command ...
class LevelingSession {
public:
    LevelingSession (){}
    LevelingSession(QVector<TrackLevelingSolver::Actuator> acts,
                    TrackLevelingSolver::Params params = TrackLevelingSolver::Params{},
                    double tol = 0.1,
                    double firstScale = 1.0)
        : acts_(std::move(acts)), p_(params), tol_(tol), firstScale_(firstScale)
    {}

    // Feed current measurement; returns command vector for this pass (same order as acts_)
    QVector<double> next(const QVector<TrackLevelingSolver::Measure>& meas) {
        if (inTolerance(meas)) return QVector<double>(int(acts_.size()), 0.0);

        if (!hasPrev_) {
            lastCmd_ = solver_.planStep1(meas, acts_, p_, firstScale_);
            lastMeas_ = meas;
            hasPrev_ = true;
            ++passes_;
            return lastCmd_;
        } else {
            auto tunedActs = acts_;
            auto fit = solver_.fitParamsFromStep1(lastMeas_, meas, tunedActs, lastCmd_, p_);
            p_ = fit.theta;           // update global params (¦Ç, ¦ËV/¦ËH, ¦Â...)
            acts_ = tunedActs;        // update actuator gains (a,b,hLeak)
            double predMax = 0.0;
            lastCmd_ = solver_.planStep2(meas, acts_, p_, &predMax);
            lastMeas_ = meas;
            ++passes_;
            return lastCmd_;
        }
    }

    // Check if measurement already within tolerance (consistent with current Params)
    bool inTolerance(const QVector<TrackLevelingSolver::Measure>& meas) const {
        double mmax = 0.0;
        for (int i=0;i<meas.size();++i) {
            const TrackLevelingSolver::Measure& m = meas[i];

            double dL = 0.0, dR = 0.0;
            if (p_.useSuperelevation) {
                const double Sdes = p_.fixedSuperelevation ? p_.S_target : ((m.R - m.L) - m.Esup);
                const double mean = p_.keepMean ? 0.5*(m.L + m.R) : p_.targetMean;
                dL = (mean - 0.5*Sdes) - m.L;
                dR = (mean + 0.5*Sdes) - m.R;
            } else {
                dL = (p_.targetMean) - m.L;
                dR = (p_.targetMean) - m.R;
            }
            const double dC = -m.C;
            mmax = std::max(mmax, std::fabs(dL));
            mmax = std::max(mmax, std::fabs(dR));
            mmax = std::max(mmax, std::fabs(dC));
            if (p_.includeGauge) {
                const double dG = -m.G;
                mmax = std::max(mmax, std::fabs(dG));
            }
        }
        return mmax <= tol_;
    }

    int  passes() const { return passes_; }
    void setTol(double tol) { tol_ = tol; }
    void setFirstScale(double k) { firstScale_ = k; }

private:
    TrackLevelingSolver solver_;
    QVector<TrackLevelingSolver::Actuator> acts_;
    TrackLevelingSolver::Params p_;
    double tol_;
    double firstScale_;

    bool hasPrev_ = false;
    QVector<TrackLevelingSolver::Measure> lastMeas_;
    QVector<double>     lastCmd_;
    int passes_ = 0;
};

#endif // LEVELINGSESSION_H
