#include <iostream>
#include <vector>
#include <algorithm>

constexpr int MOD = 7340033;
constexpr int G   = 5;

int modExp(int a,int e){
    long long r=1,cur=a;
    while(e){
        if(e&1) r=r*cur%MOD;
        cur=cur*cur%MOD;
        e>>=1;
    }
    return int(r);
}

void ntt(std::vector<int>& a,bool inv){
    int n=a.size(),lg=__builtin_ctz(n);
    static std::vector<int> rev, rt{1,1};
    rev.resize(n);
    for(int i=0;i<n;++i) rev[i]=(rev[i>>1]>>1)|((i&1)<<(lg-1));
    rt.resize(n);
    for(int k=1;(1<<k)<n;++k){
        int w=modExp(G,(MOD-1)>>(k+1));
        for(int i=1<<(k-1);i<(1<<k);++i){
            rt[2*i]=rt[i];
            rt[2*i+1]=int(1LL*rt[i]*w%MOD);
        }
    }
    for(int i=0;i<n;++i) if(i<rev[i]) std::swap(a[i],a[rev[i]]);
    for(int len=1;len<n;len<<=1)
        for(int i=0;i<n;i+=len*2)
            for(int j=0;j<len;++j){
                int u=a[i+j];
                int v=int(1LL*a[i+j+len]*rt[len+j]%MOD);
                a[i+j]     = u+v < MOD ? u+v : u+v-MOD;
                a[i+j+len] = u-v >= 0 ? u-v : u-v+MOD;
            }
    if(inv){
        std::reverse(a.begin()+1,a.end());
        int invn=modExp(n,MOD-2);
        for(int &x:a) x=int(1LL*x*invn%MOD);
    }
}

std::vector<int> multiply(std::vector<int> A,std::vector<int> B,int keep){
    int need=int(A.size()+B.size()-1),n=1;
    while(n<keep) n<<=1;
    while(n<need) n<<=1;
    A.resize(n); B.resize(n);
    ntt(A,false); ntt(B,false);
    for(int i=0;i<n;++i) A[i]=int(1LL*A[i]*B[i]%MOD);
    ntt(A,true);
    A.resize(keep);
    return A;
}

std::vector<int> invertSeries(const std::vector<int>& P,int m){
    std::vector<int> Q{modExp(P[0],MOD-2)};
    for(int len=1;len<m;len<<=1){
        int lim=len<<1;
        std::vector<int> cut(lim);
        for(int i=0;i<(int)P.size() && i<lim;++i) cut[i]=P[i];
        auto PQ = multiply(cut,Q,lim);
        std::vector<int> twoMinusPQ(lim,0);
        twoMinusPQ[0]=(2-PQ[0]+MOD)%MOD;
        for(int i=1;i<lim;++i) twoMinusPQ[i]=(MOD-PQ[i])%MOD;
        Q = multiply(Q,twoMinusPQ,lim);
    }
    Q.resize(m);
    return Q;
}

void readPoly(int& m,int& n,std::vector<int>& P){
    std::cin>>m>>n;
    P.resize(n+1);
    for(int i=0;i<=n;++i) std::cin>>P[i];
}

void printPoly(const std::vector<int>& v){
    for(size_t i=0;i<v.size();++i){
        if(i) std::cout<<' ';
        std::cout<<v[i];
    }
    std::cout<<"\n";
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int m,n;
    std::vector<int> P;
    readPoly(m,n,P);
    if(P[0]==0){ std::cout<<"The ears of a dead donkey\n"; return 0; }

    auto Q = invertSeries(P,m);
    printPoly(Q);
    return 0;
}
