#include "modules/proto_engine/expr_eval.hpp"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonValue>

static bool toBool(const QVariant& v){
    if (v.type()==QVariant::Bool) return v.toBool();
    if (v.canConvert(QVariant::LongLong)) return v.toLongLong()!=0;
    if (v.type()==QVariant::String){
        const QString s=v.toString().trimmed().toLower();
        return (s=="true"||s=="1"||s=="yes"||s=="on");
    }
    return !v.isNull();
}

long long ExprEval::nowMs(){
    return QDateTime::currentMSecsSinceEpoch();
}

QString ExprEval::trimWS(const QString& s){ return s.trimmed(); }

QVariant ExprEval::getVar(const QString& path, const QJsonObject& ctx, bool* ok) const {
    if (ok) *ok=false;
    QStringList seg = path.split('.', QString::SkipEmptyParts);
    if (seg.isEmpty()) return QVariant();
    QJsonValue cur;
    if (seg[0]=="msg") cur = ctx.value("msg");
    else if (seg[0]=="ctx") cur = QJsonValue(ctx);
    else cur = ctx.value(seg[0]);
    for (int i=1;i<seg.size();++i){
        if (!cur.isObject()) return QVariant();
        cur = cur.toObject().value(seg[i]);
    }
    if (cur.isUndefined()) return QVariant();
    if (ok) *ok=true;
    if (cur.isBool()||cur.isDouble()||cur.isString()||cur.isNull()) return cur.toVariant();
    if (cur.isObject()){
        QJsonDocument jd(cur.toObject());
        return QString::fromUtf8(jd.toJson(QJsonDocument::Compact));
    }
    if (cur.isArray()){
        QJsonDocument jd(cur.toArray());
        return QString::fromUtf8(jd.toJson(QJsonDocument::Compact));
    }
    return QVariant();
}

QVariant ExprEval::callFunc(const QString& name, const QStringList& args, bool* ok) const {
    if (ok) *ok=false;
    if (name=="now_ms" && args.size()==0){ if (ok)*ok=true; return (qlonglong)nowMs(); }
    if (name=="data" && args.size()==1){
        QString s=args[0];
        if ((s.startsWith('"')&&s.endsWith('"'))||(s.startsWith('\'')&&s.endsWith('\'')))
            s=s.mid(1, s.length()-2);
        if (prov_){ if (ok)*ok=true; return prov_->get(s); }
    }
    return QVariant();
}

QVariant ExprEval::eval(const QString& expr, const QJsonObject& ctx, bool* ok) const {
    QString e = trimWS(expr);
    int q=e.indexOf('?');
    if (q>=0){
        int c=e.indexOf(':', q+1);
        if (c>q){
            bool ok1=false,ok2=false,ok3=false;
            QVariant cond=evalSimple(e.left(q), ctx, &ok1);
            QVariant vT  =evalSimple(e.mid(q+1, c-q-1), ctx, &ok2);
            QVariant vF  =evalSimple(e.mid(c+1), ctx, &ok3);
            if (ok) *ok=(ok1&&ok2&&ok3);
            return toBool(cond)?vT:vF;
        }
    }
    return evalSimple(e, ctx, ok);
}

QVariant ExprEval::evalSimple(const QString& e, const QJsonObject& ctx, bool* ok) const {
    QVariant v = evalRelation(e, ctx, ok);
    if (ok && *ok) return v;
    return evalArith(e, ctx, ok);
}

QVariant ExprEval::evalRelation(const QString& e, const QJsonObject& ctx, bool* ok) const {
    if (ok) *ok=false;
    int p=e.indexOf("=="); bool isEq=true;
    if (p<0){ p=e.indexOf("!="); isEq=false; }
    if (p>=0){
        QString L=trimWS(e.left(p)), R=trimWS(e.mid(p+2));
        bool ok1=false,ok2=false;
        QVariant vL=evalArith(L,ctx,&ok1), vR=evalArith(R,ctx,&ok2);
        if (ok) *ok=(ok1&&ok2);
        if (!ok1||!ok2) return false;
        if (vL.type()==QVariant::String || vR.type()==QVariant::String){
            bool eq = vL.toString()==vR.toString();
            return isEq?eq:!eq;
        }
        bool eq = (vL.toLongLong()==vR.toLongLong());
        return isEq?eq:!eq;
    }
    return QVariant();
}

QVariant ExprEval::evalArith(const QString& e0, const QJsonObject& ctx, bool* ok) const {
    if (ok) *ok=false;
    QString e=trimWS(e0);
    if (e=="true"||e=="false"){ if (ok)*ok=true; return e=="true"; }
    bool on=false; qlonglong iv=e.toLongLong(&on,0);
    if (on){ if (ok)*ok=true; return iv; }
    if ((e.startsWith('"')&&e.endsWith('"'))||(e.startsWith('\'')&&e.endsWith('\''))){
        if (ok)*ok=true; return e.mid(1, e.length()-2);
    }
    int lp=e.indexOf('('), rp=e.endsWith(')')?e.length()-1:-1;
    if (lp>0 && rp>lp){
        QString fn=trimWS(e.left(lp));
        QString as=e.mid(lp+1, rp-lp-1);
        QStringList args;
        if (!as.trimmed().isEmpty()){
            foreach(QString part, as.split(',', QString::SkipEmptyParts)) args<<trimWS(part);
        }
        bool okf=false; QVariant fv=callFunc(fn,args,&okf);
        if (okf){ if (ok)*ok=true; return fv; }
    }
    bool okv=false; QVariant vv=getVar(e,ctx,&okv);
    if (okv){ if (ok)*ok=true; return vv; }
    int opPos=-1; QChar op;
    const char ops[]={'+','-','*','/'};
    for (int i=0;i<4;++i){ int k=e.indexOf(ops[i]); if (k>0){ opPos=k; op=ops[i]; break; } }
    if (opPos>0){
        QString L=trimWS(e.left(opPos)), R=trimWS(e.mid(opPos+1));
        bool okl=false,okr=false;
        qlonglong a=evalArith(L,ctx,&okl).toLongLong();
        qlonglong b=evalArith(R,ctx,&okr).toLongLong();
        if (!(okl&&okr)){ if (ok)*ok=false; return QVariant(); }
        qlonglong c=0;
        if (op=='+') c=a+b; else if (op=='-') c=a-b; else if (op=='*') c=a*b; else if (op=='/') c=(b==0?0:(a/b));
        if (ok)*ok=true; return c;
    }
    if (ok)*ok=true;
    return QVariant(e);
}
