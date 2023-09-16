/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2007       Bastian Wemmenhove
 *  Copyright (C) 2007-2010  Joris Mooij  [joris dot mooij at libdai dot org]
 *  Copyright (C) 2007       Radboud University Nijmegen, The Netherlands
 */


#include <cstdio>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <dai/mr.h>
#include <dai/bp.h>
#include <dai/jtree.h>
#include <dai/util.h>


namespace dai {


using namespace std;


const char *MR::Name = "MR";


void MR::setProperties( const PropertySet &opts ) {
    DAI_ASSERT( opts.hasKey("tol") );
    DAI_ASSERT( opts.hasKey("verbose") );
    DAI_ASSERT( opts.hasKey("updates") );
    DAI_ASSERT( opts.hasKey("inits") );

    props.tol = opts.getStringAs<Real>("tol");
    props.verbose = opts.getStringAs<size_t>("verbose");
    props.updates = opts.getStringAs<Properties::UpdateType>("updates");
    props.inits = opts.getStringAs<Properties::InitType>("inits");
}


PropertySet MR::getProperties() const {
    PropertySet opts;
    opts.Set( "tol", props.tol );
    opts.Set( "verbose", props.verbose );
    opts.Set( "updates", props.updates );
    opts.Set( "inits", props.inits );
    return opts;
}


string MR::printProperties() const {
    stringstream s( stringstream::out );
    s << "[";
    s << "tol=" << props.tol << ",";
    s << "verbose=" << props.verbose << ",";
    s << "updates=" << props.updates << ",";
    s << "inits=" << props.inits << "]";
    return s.str();
}


void MR::init(size_t Nin, Real *_w, Real *_th) {
    size_t i,j;

    N = Nin;

    con.resize(N);
    nb.resize(N);
    tJ.resize(N);
    for(i=0; i<N; i++ ) {
        nb[i].resize(kmax);
        tJ[i].resize(kmax);
        con[i]=0;
        for(j=0; j<N; j++ )
            if( _w[i*N+j] != 0.0 ) {
                nb[i][con[i]] = j;
                tJ[i][con[i]] = tanh(_w[i*N+j]);
                con[i]++;
            }
    }

    theta.resize(N);
    for(i=0; i<N; i++)
      theta[i] = _th[i];
}


Real MR::init_cor_resp() {
    size_t j,k,l, runx,i2;
    Real variab1, variab2;
    Real md, maxdev;
    Real thbJsite[kmax];
    Real xinter;
    Real rinter;
    Real res[kmax];
    size_t s2;
    size_t flag;
    size_t concav;
    size_t runs = 3000;
    Real eps = 0.2;
    size_t cavity;

    vector<vector<Real> > tJ_org;
    vector<vector<size_t> > nb_org;
    vector<size_t> con_org;
    vector<Real> theta_org;

    vector<Real> xfield(N*kmax,0.0);
    vector<Real> rfield(N*kmax,0.0);
    vector<Real> Hfield(N,0.0);
    vector<Real> devs(N*kmax,0.0);
    vector<Real> devs2(N*kmax,0.0);
    vector<Real> dev(N,0.0);
    vector<Real> avmag(N,0.0);

    // save original tJ, nb
    nb_org = nb;
    tJ_org = tJ;
    con_org = con;
    theta_org = theta;

    maxdev = 0.0;
    for(cavity=0; cavity<N; cavity++){    // for each spin to be removed
        con = con_org;
        concav=con[cavity];

        nb = nb_org;
        tJ = tJ_org;

        //  Adapt the graph variables nb[], tJ[] and con[]
        for(size_t i=0; i<con[cavity]; i++) {
            size_t ij = nb[cavity][i];
            flag=0;
            j=0;
            do{
                if(nb[ij][j]==cavity){
                    while(j<(con[ij]-1)){
                        nb[ij][j]=nb[ij][j+1];
                        tJ[ij][j] = tJ[ij][j+1];
                        j++;
                    }
                flag=1;
                }
                j++;
            } while(flag==0);
        }
        for(size_t i=0; i<con[cavity]; i++)
            con[nb[cavity][i]]--;
        con[cavity] = 0;


        // Do everything starting from the new graph********

        makekindex();
        theta = theta_org;

        for(size_t i=0; i<kmax*N; i++)
            xfield[i] = 3.0*(2*rnd_uniform()-1.);

        for(i2=0; i2<concav; i2++){ // Subsequently apply a field to each cavity spin ****************

            s2 = nb[cavity][i2];    // identify the index of the cavity spin
            for(size_t i=0; i<con[s2]; i++)
                rfield[kmax*s2+i] = 1.;

            runx=0;
            do {      // From here start the response and belief propagation
                runx++;
                md=0.0;
                for(k=0; k<N; k++){
                    if(k!=cavity) {
                        for(size_t i=0; i<con[k]; i++)
                            thbJsite[i] = tJ[k][i];
                        for(l=0; l<con[k]; l++){
                            xinter = 1.;
                            rinter = 0.;
                            if(k==s2) rinter += 1.;
                            for(j=0; j<con[k]; j++)
                                if(j!=l){
                                    variab2 = tanh(xfield[kmax*nb[k][j]+kindex[k][j]]);
                                    variab1 = thbJsite[j]*variab2;
                                    xinter *= (1.+variab1)/(1.-variab1);

                                    rinter += thbJsite[j]*rfield[kmax*nb[k][j]+kindex[k][j]]*(1-variab2*variab2)/(1-variab1*variab1);
                                }

                            variab1 = 0.5*log(xinter);
                            xinter = variab1 + theta[k];
                            devs[kmax*k+l] = xinter-xfield[kmax*k+l];
                            xfield[kmax*k+l] = xfield[kmax*k+l]+devs[kmax*k+l]*eps;
                            if( fabs(devs[kmax*k+l]) > md )
                                md = fabs(devs[kmax*k+l]);

                            devs2[kmax*k+l] = rinter-rfield[kmax*k+l];
                            rfield[kmax*k+l] = rfield[kmax*k+l]+devs2[kmax*k+l]*eps;
                            if( fabs(devs2[kmax*k+l]) > md )
                                md = fabs(devs2[kmax*k+l]);
                        }
                    }
                }
            } while((md > props.tol)&&(runx<runs)); // Precision condition reached -> BP and RP finished
            if(runx==runs)
                if( props.verbose >= 2 )
                    cerr << "init_cor_resp: Convergence not reached (md=" << md << ")..." << endl;
            if(md > maxdev)
                maxdev = md;

            // compute the observables (i.e. magnetizations and responses)******

            for(size_t i=0; i<concav; i++){
                rinter = 0.;
                xinter = 1.;
                if(i!=i2)
                    for(j=0; j<con[nb[cavity][i]]; j++){
                        variab2 = tanh(xfield[kmax*nb[nb[cavity][i]][j]+kindex[nb[cavity][i]][j]]);
                        variab1 = tJ[nb[cavity][i]][j]*variab2;
                        rinter +=  tJ[nb[cavity][i]][j]*rfield[kmax*nb[nb[cavity][i]][j]+kindex[nb[cavity][i]][j]]*(1-variab2*variab2)/(1-variab1*variab1);
                        xinter *= (1.+variab1)/(1.-variab1);
                    }
                xinter = tanh(0.5*log(xinter)+theta[nb[cavity][i]]);
                res[i] = rinter*(1-xinter*xinter);
            }

            // *******************

            for(size_t i=0; i<concav; i++)
                if(nb[cavity][i]!=s2)
            //      if(i!=i2)
                    cors[cavity][i2][i] = res[i];
                else
                    cors[cavity][i2][i] = 0;
        } // close for i2 = 0...concav
    }

    // restore nb, tJ, con
    tJ = tJ_org;
    nb = nb_org;
    con = con_org;
    theta = theta_org;

    return maxdev;
}


Real MR::T(size_t i, sub_nb A) {
    sub_nb _nbi_min_A(con[i]);
    _nbi_min_A.set();
    _nbi_min_A &= ~A;

    Real res = theta[i];
    for( size_t _j = 0; _j < _nbi_min_A.size(); _j++ )
        if( _nbi_min_A.test(_j) )
            res += atanh(tJ[i][_j] * M[i][_j]);
    return tanh(res);
}


Real MR::T(size_t i, size_t _j) {
    sub_nb j(con[i]);
    j.set(_j);
    return T(i,j);
}


Real MR::Omega(size_t i, size_t _j, size_t _l) {
    sub_nb jl(con[i]);
    jl.set(_j);
    jl.set(_l);
    Real Tijl = T(i,jl);
    return Tijl / (1.0 + tJ[i][_l] * M[i][_l] * Tijl);
}


Real MR::Gamma(size_t i, size_t _j, size_t _l1, size_t _l2) {
    sub_nb jll(con[i]);
    jll.set(_j);
    Real Tij = T(i,jll);
    jll.set(_l1);
    jll.set(_l2);
    Real Tijll = T(i,jll);

    return (Tijll - Tij) / (1.0 + tJ[i][_l1] * tJ[i][_l2] * M[i][_l1] * M[i][_l2] + tJ[i][_l1] * M[i][_l1] * Tijll + tJ[i][_l2] * M[i][_l2] * Tijll);
}


Real MR::Gamma(size_t i, size_t _l1, size_t _l2) {
    sub_nb ll(con[i]);
    Real Ti = T(i,ll);
    ll.set(_l1);
    ll.set(_l2);
    Real Till = T(i,ll);

    return (Till - Ti) / (1.0 + tJ[i][_l1] * tJ[i][_l2] * M[i][_l1] * M[i][_l2] + tJ[i][_l1] * M[i][_l1] * Till + tJ[i][_l2] * M[i][_l2] * Till);
}


Real MR::_tJ(size_t i, sub_nb A) {
    sub_nb::size_type _j = A.find_first();
    if( _j == sub_nb::npos )
        return 1.0;
    else
        return tJ[i][_j] * _tJ