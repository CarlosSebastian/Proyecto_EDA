#include "../distance/Bounds.h"
#include <algorithm>
#include <limits>

// Inicialización de variables estáticas
std::deque<int> Bounds::du;
std::deque<int> Bounds::dl;
std::vector<double> Bounds::proj;
std::vector<double> Bounds::up;
std::vector<double> Bounds::lp;

double Bounds::lbKeogh(const std::vector<double>& q, const std::vector<double>& u, 
                       const std::vector<double>& l, double bsf) {
    double lb = 0.0;
    for (size_t i = 0; i < q.size() && lb <= bsf; i++) {
        double qi = q[i];
        if (qi > u[i]) {
            lb += dist(qi, u[i]);
        } else if (qi < l[i]) {
            lb += dist(qi, l[i]);
        }
    }
    return lb;
}

double Bounds::lbEnhanced(const std::vector<double>& q, const std::vector<double>& t,
                         const std::vector<double>& u, const std::vector<double>& l, 
                         int k, int w, double bsf) {
    double lb = 0.0;
    
    if (k > static_cast<int>(q.size() / 2)) k = q.size() / 2;
    
    if (k > 0) {
        double q0 = q[0];
        double qe0 = q[q.size() - 1];
        double t0 = t[0];
        double te0 = t[t.size() - 1];
        
        lb = dist(q0, t0) + dist(qe0, te0);
        
        for (int i = 1; i < k && lb <= bsf; i++) {
            double mind, minde;
            int startj;
            double ti = t[i];
            double qi = q[i];
            double tei = t[t.size() - 1 - i];
            double qei = q[q.size() - 1 - i];
            
            if (i <= w) {
                mind = std::min(dist(q0, ti), std::min(dist(t0, qi), dist(qi, ti)));
                minde = std::min(dist(qe0, tei), std::min(dist(te0, qei), dist(tei, qei)));
                startj = 1;
            } else {
                mind = dist(qi, ti);
                minde = dist(tei, qei);
                startj = i - w;
            }
            
            for (int j = startj; j < i; j++) {
                double d = std::min(dist(qi, t[j]), dist(ti, q[j]));
                if (d < mind) mind = d;
                
                double de = std::min(dist(qei, t[t.size() - 1 - j]), dist(tei, q[q.size() - 1 - j]));
                if (de < minde) minde = de;
            }
            
            lb += mind + minde;
        }
    }
    
    for (size_t i = k; i < q.size() - k && lb <= bsf; i++) {
        double qi = q[i];
        if (qi > u[i]) {
            lb += dist(qi, u[i]);
        } else if (qi < l[i]) {
            lb += dist(qi, l[i]);
        }
    }
    
    return lb;
}

double Bounds::lbImproved(const std::vector<double>& q, const std::vector<double>& t,
                         const std::vector<double>& ut, const std::vector<double>& lt, 
                         int w, double bsf) {
    double lb = 0.0;
    
    if (proj.size() < q.size()) {
        proj.resize(q.size());
    }
    
    for (size_t i = 0; i < q.size() && lb <= bsf; i++) {
        double qi = q[i];
        if (qi > ut[i]) {
            lb += dist(qi, ut[i]);
            proj[i] = ut[i];
        } else if (qi < lt[i]) {
            lb += dist(qi, lt[i]);
            proj[i] = lt[i];
        } else {
            proj[i] = qi;
        }
    }
    
    if (lb > bsf) return lb;
    
    if (up.size() < q.size()) {
        up.resize(q.size());
        lp.resize(q.size());
    }
    
    lemireGetEnvelopes(proj, w, up, lp);
    lb += lbKeogh(t, up, lp, bsf);
    
    return lb;
}

double Bounds::lbPetitjean(const std::vector<double>& q, const std::vector<double>& uq,
                          const std::vector<double>& lq, const std::vector<double>& t,
                          const std::vector<double>& ut, const std::vector<double>& lt, 
                          int w, double bsf) {
    double lb = 0;
    int istart = 0;
    
    if (w >= 1 && q.size() >= 6) {
        double q0 = q[0], t0 = t[0], qe0 = q[q.size()-1], te0 = t[t.size()-1];
        double q1 = q[1], t1 = t[1], t2 = t[2], q2 = q[2];
        
        double d01 = dist(q0, t1), d11 = dist(q1, t1), d10 = dist(q1, t0);
        
        if (w == 1) {
            lb = dist(q0, t0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01, d11) + dist(q1, t2),
                std::min(d10, d11) + dist(q2, t1)
            });
        } else {
            lb = dist(q0, t0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01 + dist(q0, t2), std::min(d01, d11) + dist(q1, t2)),
                std::min(d10 + dist(q2, t0), std::min(d10, d11) + dist(q2, t1))
            });
        }
        
        if (lb > bsf) return lb;
        
        q1 = q[q.size()-2];
        t1 = t[t.size()-2];
        t2 = t[t.size()-3];
        q2 = q[q.size()-3];
        
        d01 = dist(qe0, t1);
        d11 = dist(q1, t1);
        d10 = dist(q1, te0);
        
        if (w == 1) {
            lb += dist(qe0, te0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01, d11) + dist(q1, t2),
                std::min(d10, d11) + dist(q2, t1)
            });
        } else {
            lb += dist(qe0, te0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01 + dist(qe0, t2), std::min(d01, d11) + dist(q1, t2)),
                std::min(d10 + dist(q2, te0), std::min(d10, d11) + dist(q2, t1))
            });
        }
        
        if (lb > bsf) return lb;
        istart = 3;
    }
    
    if (proj.size() < q.size()) proj.resize(q.size());
    
    for (size_t i = istart; i < q.size() - istart && lb <= bsf; i++) {
        double qi = q[i];
        if (qi > ut[i]) {
            lb += dist(qi, ut[i]);
            proj[i] = ut[i];
        } else if (qi < lt[i]) {
            lb += dist(qi, lt[i]);
            proj[i] = lt[i];
        } else {
            proj[i] = qi;
        }
    }
    
    if (lb > bsf) return lb;
    
    for (int i = 0; i < istart; i++) {
        proj[i] = q[i];
        proj[q.size() - i - 1] = q[q.size() - i - 1];
    }
    
    if (up.size() < q.size()) {
        up.resize(q.size());
        lp.resize(q.size());
    }
    
    lemireGetEnvelopes(proj, w, up, lp);
    
    for (size_t i = istart; i < t.size() - istart && lb <= bsf; i++) {
        double ti = t[i];
        if (ti > up[i]) {
            if (up[i] > uq[i]) {
                lb += dist(ti, uq[i]) - dist(up[i], uq[i]);
            } else {
                lb += dist(ti, up[i]);
            }
        } else if (ti < lp[i]) {
            if (lp[i] < lq[i]) {
                lb += dist(ti, lq[i]) - dist(lp[i], lq[i]);
            } else {
                lb += dist(ti, lp[i]);
            }
        }
    }
    
    return lb;
}

double Bounds::lbWebb(const std::vector<double>& q, const std::vector<double>& uq,
                     const std::vector<double>& lq, const std::vector<double>& luq,
                     const std::vector<double>& ulq, const std::vector<double>& t,
                     const std::vector<double>& ut, const std::vector<double>& lt,
                     const std::vector<double>& lut, const std::vector<double>& ult,
                     int window, double bsf) {
    double lb = 0;
    int istart = 0;
    
    if (window >= 1 && q.size() >= 6) {
        double q0 = q[0], t0 = t[0], qe0 = q[q.size()-1], te0 = t[t.size()-1];
        double q1 = q[1], t1 = t[1], t2 = t[2], q2 = q[2];
        
        double d01 = dist(q0, t1), d11 = dist(q1, t1), d10 = dist(q1, t0);
        
        if (window == 1) {
            lb = dist(q0, t0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01, d11) + dist(q1, t2),
                std::min(d10, d11) + dist(q2, t1)
            });
        } else {
            lb = dist(q0, t0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01 + dist(q0, t2), std::min(d01, d11) + dist(q1, t2)),
                std::min(d10 + dist(q2, t0), std::min(d10, d11) + dist(q2, t1))
            });
        }
        
        if (lb > bsf) return lb;
        
        q1 = q[q.size()-2];
        t1 = t[t.size()-2];
        t2 = t[t.size()-3];
        q2 = q[q.size()-3];
        
        d01 = dist(qe0, t1);
        d11 = dist(q1, t1);
        d10 = dist(q1, te0);
        
        if (window == 1) {
            lb += dist(qe0, te0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01, d11) + dist(q1, t2),
                std::min(d10, d11) + dist(q2, t1)
            });
        } else {
            lb += dist(qe0, te0) + std::min({
                d11 + dist(q2, t2),
                std::min(d01 + dist(qe0, t2), std::min(d01, d11) + dist(q1, t2)),
                std::min(d10 + dist(q2, te0), std::min(d10, d11) + dist(q2, t1))
            });
        }
        
        if (lb > bsf) return lb;
        istart = 3;
    }
    
    int freeCountAbove = window;
    int freeCountBelow = window;
    int qEnd = q.size() - istart;
    
    for (int i = istart; i < qEnd && lb <= bsf; i++) {
        double qi = q[i];
        
        if (qi > ut[i]) {
            lb += dist(qi, ut[i]);
            freeCountBelow = (ut[i] >= ulq[i]) ? freeCountBelow + 1 : 0;
            freeCountAbove = 0;
        } else if (qi < lt[i]) {
            lb += dist(qi, lt[i]);
            freeCountAbove = (lt[i] <= luq[i]) ? freeCountAbove + 1 : 0;
            freeCountBelow = 0;
        } else {
            freeCountAbove++;
            freeCountBelow++;
        }
        
        if (i >= window + istart) {
            int j = i - window;
            double tj = t[j];
            double uqj = uq[j];
            
            if (tj > uqj) {
                if (freeCountAbove > 2 * window) {
                    lb += dist(tj, uqj);
                } else {
                    double ultj = ult[j];
                    if (tj > ultj && ultj >= uqj) {
                        lb += dist(tj, uqj) - dist(ultj, uqj);
                    }
                }
            } else {
                double lqj = lq[j];
                if (tj < lqj) {
                    if (freeCountBelow > 2 * window) {
                        lb += dist(tj, lqj);
                    } else {
                        double lutj = lut[j];
                        if (tj < lutj && lutj <= lqj) {
                            lb += dist(tj, lqj) - dist(lutj, lqj);
                        }
                    }
                }
            }
        }
    }
    
    for (int j = qEnd - window; j < qEnd && lb <= bsf; j++) {
        double tj = t[j];
        double uqj = uq[j];
        
        if (tj > uqj) {
            if (j >= qEnd - freeCountAbove + window) {
                lb += dist(tj, uqj);
            } else {
                double ultj = ult[j];
                if (tj > ultj && ultj >= uqj) {
                    lb += dist(tj, uqj) - dist(ultj, uqj);
                }
            }
        } else {
            double lqj = lq[j];
            if (tj < lqj) {
                if (j >= qEnd - freeCountBelow + window) {
                    lb += dist(tj, lqj);
                } else {
                    double lutj = lut[j];
                    if (tj < lutj && lutj <= lqj) {
                        lb += dist(tj, lqj) - dist(lutj, lqj);
                    }
                }
            }
        }
    }
    
    return lb;
}

double Bounds::enhancedLBWebb(const std::vector<double>& q, const std::vector<double>& uq,
                             const std::vector<double>& lq, const std::vector<double>& luq,
                             const std::vector<double>& ulq, const std::vector<double>& t,
                             const std::vector<double>& ut, const std::vector<double>& lt,
                             const std::vector<double>& lut, const std::vector<double>& ult,
                             int k, int window, double bsf) {
    double lb = 0.0;
    
    if (k > static_cast<int>(q.size() / 2)) k = q.size() / 2;
    
    if (k > 0) {
        double q0 = q[0], qe0 = q[q.size()-1], t0 = t[0], te0 = t[t.size()-1];
        lb = dist(q0, t0) + dist(qe0, te0);
        
        for (int i = 1; i < k && lb <= bsf; i++) {
            double mind, minde;
            int startj;
            double ti = t[i], qi = q[i], tei = t[t.size()-1-i], qei = q[q.size()-1-i];
            
            if (i <= window) {
                mind = std::min(dist(q0, ti), std::min(dist(t0, qi), dist(qi, ti)));
                minde = std::min(dist(qe0, tei), std::min(dist(te0, qei), dist(tei, qei)));
                startj = 1;
            } else {
                mind = dist(qi, ti);
                minde = dist(tei, qei);
                startj = i - window;
            }
            
            for (int j = startj; j < i; j++) {
                double d = std::min(dist(qi, t[j]), dist(ti, q[j]));
                if (d < mind) mind = d;
                
                double de = std::min(dist(qei, t[t.size()-1-j]), dist(tei, q[q.size()-1-j]));
                if (de < minde) minde = de;
            }
            
            lb += mind + minde;
        }
    }
    
    if (lb > bsf) return lb;
    
    int istart = k;
    int freeCountAbove = window, freeCountBelow = window;
    int qEnd = q.size() - istart;
    
    for (int i = istart; i < qEnd && lb <= bsf; i++) {
        double qi = q[i];
        
        if (qi > ut[i]) {
            lb += dist(qi, ut[i]);
            freeCountBelow = (ut[i] >= ulq[i]) ? freeCountBelow + 1 : 0;
            freeCountAbove = 0;
        } else if (qi < lt[i]) {
            lb += dist(qi, lt[i]);
            freeCountAbove = (lt[i] <= luq[i]) ? freeCountAbove + 1 : 0;
            freeCountBelow = 0;
        } else {
            freeCountAbove++;
            freeCountBelow++;
        }
        
        if (i >= window + istart) {
            int j = i - window;
            double tj = t[j], uqj = uq[j];
            
            if (tj > uqj) {
                if (freeCountAbove > 2 * window) {
                    lb += dist(tj, uqj);
                } else {
                    double ultj = ult[j];
                    if (tj > ultj && ultj >= uqj) {
                        lb += dist(tj, uqj) - dist(ultj, uqj);
                    }
                }
            } else {
                double lqj = lq[j];
                if (tj < lqj) {
                    if (freeCountBelow > 2 * window) {
                        lb += dist(tj, lqj);
                    } else {
                        double lutj = lut[j];
                        if (tj < lutj && lutj <= lqj) {
                            lb += dist(tj, lqj) - dist(lutj, lqj);
                        }
                    }
                }
            }
        }
    }
    
    for (int j = qEnd - window; j < qEnd && lb <= bsf; j++) {
        double tj = t[j], uqj = uq[j];
        
        if (tj > uqj) {
            if (j >= qEnd - freeCountAbove + window) {
                lb += dist(tj, uqj);
            } else {
                double ultj = ult[j];
                if (tj > ultj && ultj >= uqj) {
                    lb += dist(tj, uqj) - dist(ultj, uqj);
                }
            }
        } else {
            double lqj = lq[j];
            if (tj < lqj) {
                if (j >= qEnd - freeCountBelow + window) {
                    lb += dist(tj, lqj);
                } else {
                    double lutj = lut[j];
                    if (tj < lutj && lutj <= lqj) {
                        lb += dist(tj, lqj) - dist(lutj, lqj);
                    }
                }
            }
        }
    }
    
    return lb;
}

void Bounds::simpleGetEnvelopes(const std::vector<double>& t, int w,
                               std::vector<double>& u, std::vector<double>& l) {
    for (size_t i = 0; i < t.size(); i++) {
        double maxVal = -std::numeric_limits<double>::max();
        double minVal = std::numeric_limits<double>::max();
        
        int start = std::max(0, static_cast<int>(i) - w);
        int end = std::min(static_cast<int>(t.size()) - 1, static_cast<int>(i) + w);
        
        for (int j = start; j <= end; j++) {
            if (t[j] > maxVal) maxVal = t[j];
            if (t[j] < minVal) minVal = t[j];
        }
        
        u[i] = maxVal;
        l[i] = minVal;
    }
}

void Bounds::lemireGetEnvelopes(const std::vector<double>& t, int w,
                               std::vector<double>& u, std::vector<double>& l) {
    du.clear();
    dl.clear();
    
    du.push_back(0);
    dl.push_back(0);
    
    if (w >= static_cast<int>(t.size())) w = t.size() - 1;
    
    for (int i = 1; i <= w; i++) {
        if (t[i] > t[i-1]) {
            du.pop_back();
            while (!du.empty() && t[i] > t[du.back()]) du.pop_back();
        } else {
            dl.pop_back();
            while (!dl.empty() && t[i] <= t[dl.back()]) dl.pop_back();
        }
        du.push_back(i);
        dl.push_back(i);
    }
    
    int duFirst = du.front();
    int dlFirst = dl.front();
    
    for (int i = w+1; i < static_cast<int>(t.size()); i++) {
        u[i-w-1] = t[duFirst];
        l[i-w-1] = t[dlFirst];
        
        if (t[i] > t[i-1]) {
            du.pop_back();
            while (!du.empty() && t[i] > t[du.back()]) du.pop_back();
            if (du.empty()) duFirst = i;
        } else {
            dl.pop_back();
            while (!dl.empty() && t[i] <= t[dl.back()]) dl.pop_back();
            if (dl.empty()) dlFirst = i;
        }
        du.push_back(i);
        dl.push_back(i);
        
        if (i == 2*w + 1 + duFirst) {
            du.pop_front();
            duFirst = du.front();
        } else if (i == 2*w + 1 + dlFirst) {
            dl.pop_front();
            dlFirst = dl.front();
        }
    }
    
    for (int i = t.size()-w-1; i < static_cast<int>(t.size())-1; i++) {
        u[i] = t[duFirst];
        l[i] = t[dlFirst];
        if (i - duFirst >= w) {
            du.pop_front();
            duFirst = du.front();
        }
        if (i - dlFirst >= w) {
            dl.pop_front();
            dlFirst = dl.front();
        }
    }
    
    u[t.size()-1] = t[duFirst];
    l[t.size()-1] = t[dlFirst];
}

void Bounds::lemireGetUpper(const std::vector<double>& t, int w, std::vector<double>& u) {
    du.clear();
    du.push_back(0);
    
    if (w >= static_cast<int>(t.size())) w = t.size() - 1;
    
    for (int i = 1; i <= w; i++) {
        if (t[i] > t[i-1]) {
            du.pop_back();
            while (!du.empty() && t[i] > t[du.back()]) du.pop_back();
        }
        du.push_back(i);
    }
    
    int duFirst = du.front();
    
    for (int i = w+1; i < static_cast<int>(t.size()); i++) {
        u[i-w-1] = t[duFirst];
        
        if (t[i] > t[i-1]) {
            du.pop_back();
            while (!du.empty() && t[i] > t[du.back()]) du.pop_back();
            if (du.empty()) duFirst = i;
        }
        du.push_back(i);
        if (i == 2*w + 1 + duFirst) {
            du.pop_front();
            duFirst = du.front();
        }
    }
    
    for (int i = t.size()-w-1; i < static_cast<int>(t.size())-1; i++) {
        u[i] = t[duFirst];
        if (i - duFirst >= w) {
            du.pop_front();
            duFirst = du.front();
        }
    }
    
    u[t.size()-1] = t[duFirst];
}

void Bounds::lemireGetLower(const std::vector<double>& t, int w, std::vector<double>& l) {
    dl.clear();
    dl.push_back(0);
    
    if (w >= static_cast<int>(t.size())) w = t.size() - 1;
    
    for (int i = 1; i <= w; i++) {
        if (t[i] < t[i-1]) {
            dl.pop_back();
            while (!dl.empty() && t[i] <= t[dl.back()]) dl.pop_back();
        }
        dl.push_back(i);
    }
    
    int dlFirst = dl.front();
    
    for (int i = w+1; i < static_cast<int>(t.size()); i++) {
        l[i-w-1] = t[dlFirst];
        
        if (t[i] < t[i-1]) {
            dl.pop_back();
            while (!dl.empty() && t[i] <= t[dl.back()]) dl.pop_back();
            if (dl.empty()) dlFirst = i;
        }
        dl.push_back(i);
        if (i == 2*w + 1 + dlFirst) {
            dl.pop_front();
            dlFirst = dl.front();
        }
    }
    
    for (int i = t.size()-w-1; i < static_cast<int>(t.size())-1; i++) {
        l[i] = t[dlFirst];
        if (i - dlFirst >= w) {
            dl.pop_front();
            dlFirst = dl.front();
        }
    }
    
    l[t.size()-1] = t[dlFirst];
}