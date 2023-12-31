#include "AssLinSchur.h"

#include <armadillo>
#include "Const.h"
#include "Mem.h"
#include "EqSys.h"
#include "DoF.h"
#include "EleMat.h"
#include "Eigen.h"

#include <cfloat>

AssLinSchur::AssLinSchur(std::ofstream& logFile, EqSys* sys): prj(sys->prj), msh(sys->msh), opt(sys->opt), quad(sys->quad)
{
//    double phase[] = {1.0,1.0,1.0,1.0,-1.0,
//                      1.0,-1.0,1.0,1.0,1.0,
//                      1.0,1.0,-1.0,-1.0,1.0,
//                      1.0,1.0,-1.0,1.0,-1.0,
//                      1.0,1.0,-1.0,1.0,-1.0
//                     };
//    size_t countPort = 0;
    logFile << "% Assembly Symmetric:\n";
    logFile << "In solids: ";
    arma::wall_clock tt, lt;
    tt.tic();
    double k0 = 2.0 * Const::pi * sys->freq / Const::c0;
    double kk = k0*k0;
    sys->DoFnum = DoF(prj).DoFnumv;
    if(opt->verbose)
    {
        std::cout << "RAW FE DoF = " << sys->DoFnum << "\n";
    }
    std::vector<bool> DoFtoLeave(sys->DoFnum, true);
    /// Computing raw dof mapping
    sys->DoFmapv.resize(sys->DoFnum);
    sys->InvDoFmapv.resize(sys->DoFnum);
    arma::uvec BndDoF;
    //arma::field<arma::uvec> IntDoF;
    arma::field<std::vector<bool> > IsIntDoF;
    //IntDoF.set_size(msh->nDomains);
    IsIntDoF.set_size(msh->nDomains);
    for(size_t did = 0; did < msh->nDomains; did++)
    {
        std::vector<bool> DoFinternal(sys->DoFnum, true);
        arma::uvec bndFaces = msh->domFaces(did);
        for(size_t fif = 0; fif < bndFaces.n_rows; fif++)
        {
            DoF cDoF(prj, 2, bndFaces(fif));
            BndDoF = arma::unique(arma::join_cols(BndDoF, cDoF.v));
            for(size_t id=0; id<cDoF.v.n_rows; id++)
            {
                DoFinternal[cDoF.v(id)] = false;
            }
        }
        IsIntDoF(did) = DoFinternal;
    }
    size_t gidx = 0;
    sys->DoFlevel.push_back(gidx);
    for(size_t bib = 0; bib < BndDoF.n_rows; bib++)
    {
        sys->DoFmapv(BndDoF(bib)) = gidx++;
    }
    sys->DoFlevel.push_back(gidx);
    //std::cout << "Bnds: 0-" << gidx-1 << "\n";
    for(size_t did = 0; did < msh->nDomains; did++)
    {
        arma::uvec intTetras = msh->domTetras(did);
        std::vector<bool> DoFinternal = IsIntDoF(did);
        //std::cout << "Domain " << did << ": " << gidx << "-";
        for(size_t tit = 0; tit < intTetras.n_rows; tit++)
        {
            DoF cDoF(prj, 3, intTetras(tit));
            //IntDoF(did) = arma::unique(arma::join_cols(IntDoF(did), cDoF.v));
            for(size_t id=0; id<cDoF.v.n_rows; id++)
            {
                if(DoFinternal[cDoF.v(id)])
                {
                    sys->DoFmapv(cDoF.v(id)) = gidx++;
                    DoFinternal[cDoF.v(id)] = false;
                }
            }
        }
        //std::cout << gidx-1 << "\n";
        sys->DoFlevel.push_back(gidx);
    }
    #pragma omp parallel for
    for(size_t dofid=0; dofid<sys->DoFnum; dofid++)
    {
        sys->InvDoFmapv(sys->DoFmapv(dofid)) = dofid;
    }
    if(opt->verbose)
    {
        std::cout << "FE DoF = " << sys->DoFnum << " ";
    }
    sys->SymmFlag = 0;
    sys->A.clear_mat();
    sys->B.clear_mat();
    sys->Sol.clear();
    sys->Sp.clear();
    gmm::resize(sys->A, sys->DoFnum, sys->DoFnum);
    lt.tic();
    // prebuild AFF
    sys->AFF.resize(msh->nDomains);
    for(size_t did = 0; did < msh->nDomains; did++)
    {
        gmm::resize(sys->AFF[did],sys->DoFlevel[1], sys->DoFlevel[1]);
        //std::cout << sys->AFF[did] << "\n";
    }
    #pragma omp parallel for
    for(size_t id = 0; id < msh->nTetras; id++)
    {
        Mtrl* cMtrl = &(msh->tetMtrl[msh->tetLab(id)]);
        EleMat lMat(opt->pOrd, 3, msh->tetGeo(id), quad, cMtrl, Shape::Hcurl);
        DoF cDoF(prj, 3, id);
        cDoF.v = sys->DoFmapv.elem(cDoF.v);
        #pragma omp critical
        for(int i=0; i<cDoF.v.n_rows; i++)
        {
            for(int j=0; j<cDoF.v.n_rows; j++)
            {
                if(cDoF.v(i)<=cDoF.v(j))
                {
                    sys->A(cDoF.v(i),cDoF.v(j)) += lMat.S(i,j) + k0*lMat.Z(i,j) - kk*lMat.T(i,j);
                    if((cDoF.v(i)<sys->DoFlevel[1]) && (cDoF.v(j) < sys->DoFlevel[1]))
                    {
                        sys->AFF[msh->tetDom(id)](cDoF.v(i),cDoF.v(j)) += lMat.S(i,j) + k0*lMat.Z(i,j) - kk*lMat.T(i,j);
                    }
                }
            }
        }
    }
//    EqSys::MatRowType Test(sys->DoFlevel[1], sys->DoFlevel[1]);
//    for(size_t did = 0; did < msh->nDomains; did++) {
//        gmm::add(sys->AFF[did], Test);
//        //std::cout << sys->AFF[did] << "\n";
//    }
//    std::cout << Test;
//    gmm::add(gmm::scaled(gmm::sub_matrix(sys->A, gmm::sub_interval(0, sys->DoFlevel[1])),-1.0), Test);
//    std::cout << Test; exit(0);
//    for(size_t did = 0; did < msh->nDomains; did++) {
//        //gmm::resize(sys->AFF[did],sys->DoFlevel[1], sys->DoFlevel[1]);
//        std::cout << sys->AFF[did] << "\n";
//    }
//    for(size_t did = 0; did < msh->nDomains; did++) {
//        arma::uvec ddFaces = msh->domFaces(did);
//        std::cout << "Domain " << did << ": " << ddFaces.n_rows << "-\n";
//        for(size_t dif = 0; dif < ddFaces.n_rows; dif++) {
//            std::cout << ddFaces(dif) << "\n";
//        // get
////            DoF cDoF(prj, 3, intTetras(tit));
////            //IntDoF(did) = arma::unique(arma::join_cols(IntDoF(did), cDoF.v));
////            for(size_t id=0; id<cDoF.v.n_rows; id++) {
////                if(DoFinternal[cDoF.v(id)]) {
////                    sys->DoFmapv(cDoF.v(id)) = gidx++;
////                    DoFinternal[cDoF.v(id)] = false;
////                }
////            }
//        }
//        //std::cout << gidx-1 << "\n";
//    }
//
    logFile << lt.toc() << " s\n";
    if(opt->verbose)
    {
        std::cout << lt.toc() << " s\n";
    }
    MemStat::print(logFile);
    if(opt->verbose)
    {
        MemStat::print(std::cout);
    }
    logFile << "On boundaries:\n";
    for(size_t bcid = 0; bcid < msh->facBC.size(); bcid++)
    {
        BC* bc = &(msh->facBC[bcid]);
        switch(bc->type)
        {
        case BC::PerfectE :
            lt.tic();
            if(opt->verbose)
            {
                std::cout << bc->name;
            }
            logFile << "\t" << bc->name << ": ";
            #pragma omp parallel for
            for(size_t fid = 0; fid < bc->Faces.size(); fid++)
            {
                DoF cDoF(prj, 2, bc->Faces(fid));
                cDoF.v = sys->DoFmapv.elem(cDoF.v);
                #pragma omp critical
                {
                    sys->DirDoFs = arma::join_cols(sys->DirDoFs, cDoF.s);
                    sys->DirDoFv = arma::join_cols(sys->DirDoFv, cDoF.v);
                    for(size_t dofid=0; dofid < cDoF.v.n_rows; dofid++)
                    {
                        DoFtoLeave[cDoF.v(dofid)] = false;
                    }
                }
            }
            sys->DirDoFs = arma::unique(sys->DirDoFs);
            sys->DirDoFv = arma::unique(sys->DirDoFv);
            logFile << lt.toc() << " s\n";
            if(opt->verbose)
            {
                std::cout << " ";
            }
            break;
        case BC::WavePort:
            break;
        case BC::Radiation:
            lt.tic();
            if(opt->verbose)
            {
                std::cout << bc->name;
            }
            logFile << "\t" << bc->name << ": ";
            #pragma omp parallel for
            for(size_t fid = 0; fid < bc->Faces.size(); fid++)
            {
                arma::uvec adjTet = msh->facAdjTet(bc->Faces(fid));
                Mtrl* cMtrl = &(msh->tetMtrl[msh->tetLab(adjTet(0))]);
                std::complex<double> epsr(cMtrl->epsr, cMtrl->CalcEpsr2(sys->freq));
                double mur = cMtrl->mur;
                EleMat lMat(opt->pOrd, 2, msh->facGeo(bc->Faces(fid)), quad,
                            cMtrl, msh->intNode(bc->Faces(fid)));
                DoF cDoF(prj, 2, bc->Faces(fid));
                cDoF.v = sys->DoFmapv.elem(cDoF.v);
                #pragma omp critical
                for(int i=0; i<cDoF.v.n_rows; i++)
                {
                    for(int j=0; j<cDoF.v.n_rows; j++)
                    {
                        if(cDoF.v(i)<=cDoF.v(j))
                        {
                            sys->A(cDoF.v(i),cDoF.v(j)) += std::complex<double>(0.0,k0)*lMat.Tt(i,j)*std::sqrt(epsr/mur);
                            if((cDoF.v(i)<sys->DoFlevel[1]) && (cDoF.v(j) < sys->DoFlevel[1]))
                            {
                                sys->AFF[msh->tetDom(adjTet(0))](cDoF.v(i),cDoF.v(j)) +=
                                    std::complex<double>(0.0,k0)*lMat.Tt(i,j)*std::sqrt(epsr/mur);
                            }
                        }
                    }
                }
            }
            logFile << lt.toc() << " s\n";
            if(opt->verbose)
            {
                std::cout << " ";
            }
            break;
        case BC::PerfectH:
            lt.tic();
            if(opt->verbose)
            {
                std::cout << bc->name;
            }
            logFile << "\t" << bc->name << ": ";
            logFile << lt.toc() << " s\n";
            if(opt->verbose)
            {
                std::cout << " ";
            }
            break;
        default:
            throw std::string("Wrong boundary type");
        }
    }
    sys->WavePortsNum = 0;
    sys->WavePortsDoFnum = 0;
    sys->WavePortIds.reset();
    for(size_t bcid = 0; bcid < msh->facBC.size(); bcid++)
    {
        BC* bc = &(msh->facBC[bcid]);
        if(bc->type == BC::WavePort)
        {
            lt.tic();
            if(opt->verbose)
            {
                std::cout << bc->name;
            }
            logFile << "\t" << bc->name << ": ";
            arma::uvec tmpEdges, tmpDoFv, tmpDirDoFv;
            arma::uvec tmpNodes, tmpDoFs, tmpDirDoFs;
            arma::uvec tmp;
            tmpDoFs = arma::zeros<arma::uvec>(DoF(prj).DoFnums);
            tmpDoFv = arma::zeros<arma::uvec>(DoF(prj).DoFnumv);
            for(size_t fid = 0; fid < bc->Faces.size(); fid++)
            {
                DoF cDoF(prj, 2, bc->Faces(fid));
                cDoF.v = sys->DoFmapv.elem(cDoF.v);
                tmpEdges = arma::join_cols(tmpEdges, cDoF.v);
                tmpNodes = arma::join_cols(tmpNodes, cDoF.s);
            }
            tmpNodes = arma::unique(tmpNodes);
            tmpEdges = arma::unique(tmpEdges);
            tmp.reset();
            tmp = arma::uvec(tmpEdges.n_rows);
            tmp.fill(0);
            for(size_t i = 0; i < tmpEdges.n_rows; i++)
            {
                tmpDoFv(tmpEdges(i)) = i;
                arma::uvec pD = arma::find(sys->DirDoFv == tmpEdges(i));
                if(pD.n_rows == 0)
                {
                    tmp(i) = 1;
                }
            }
            tmpDirDoFv = arma::find(tmp > 0);
            tmp.reset();
            tmp = arma::uvec(tmpNodes.n_rows);
            tmp.fill(0);
            for(size_t i = 0; i < tmpNodes.n_rows; i++)
            {
                tmpDoFs(tmpNodes(i)) = i;
                arma::uvec pD = arma::find(sys->DirDoFs == tmpNodes(i));
                if(pD.n_rows == 0)
                {
                    tmp(i) = 1;
                }
            }
            tmpDirDoFs = arma::find(tmp > 0);
            if(opt->tfe)
            {
                for(size_t dofid=0; dofid < tmpEdges.n_rows; dofid++)
                {
                    DoFtoLeave[tmpEdges(dofid)] = false;
                }
            }
            arma::cx_mat tmpSt(tmpEdges.n_rows,tmpEdges.n_rows);
            arma::cx_mat tmpTt(tmpEdges.n_rows,tmpEdges.n_rows);
            arma::cx_mat tmpTt2(tmpEdges.n_rows,tmpEdges.n_rows);
            arma::cx_mat tmpSz(tmpNodes.n_rows,tmpNodes.n_rows);
            arma::cx_mat tmpTz(tmpNodes.n_rows,tmpNodes.n_rows);
            arma::cx_mat tmpG(tmpEdges.n_rows,tmpNodes.n_rows);
            tmpSt.fill(0);
            tmpTt.fill(0);
            tmpTt2.fill(0);
            tmpSz.fill(0);
            tmpTz.fill(0);
            tmpG.fill(0);
            double maxepsr = DBL_MIN;
            double maxmur = DBL_MIN;
            for(size_t fid = 0; fid < bc->Faces.size(); fid++)
            {
                arma::uvec adjTet = msh->facAdjTet(bc->Faces(fid));
                Mtrl* cMtrl = &(msh->tetMtrl[msh->tetLab(adjTet(0))]);
                std::complex<double> epsr(cMtrl->epsr, cMtrl->CalcEpsr2(sys->freq));
                double mur = cMtrl->mur;
                maxepsr = std::max(maxepsr, std::real(epsr));
                maxmur = std::max(maxmur, mur);
                EleMat lMat(opt->pOrd, 2, msh->facGeo(bc->Faces(fid)), quad,
                            cMtrl, msh->intNode(bc->Faces(fid)));
                DoF cDoF(prj, 2, bc->Faces(fid));
                cDoF.v = sys->DoFmapv.elem(cDoF.v);
                for(int i=0; i<cDoF.v.n_rows; i++)
                {
                    for(int j=0; j<cDoF.v.n_rows; j++)
                    {
                        tmpSt(tmpDoFv(cDoF.v(i)),tmpDoFv(cDoF.v(j))) += lMat.St(i,j)/mur;
                        tmpTt(tmpDoFv(cDoF.v(i)),tmpDoFv(cDoF.v(j))) += lMat.Tt(i,j)*epsr;
                        tmpTt2(tmpDoFv(cDoF.v(i)),tmpDoFv(cDoF.v(j))) += lMat.Tt(i,j)/mur;
                    }
                }
                for(int i=0; i<cDoF.s.n_rows; i++)
                {
                    for(int j=0; j<cDoF.s.n_rows; j++)
                    {
                        tmpSz(tmpDoFs(cDoF.s(i)),tmpDoFs(cDoF.s(j))) += lMat.Sz(i,j)/mur;
                        tmpTz(tmpDoFs(cDoF.s(i)),tmpDoFs(cDoF.s(j))) += lMat.Tz(i,j)*epsr;
                    }
                }
                for(int i=0; i<cDoF.v.n_rows; i++)
                {
                    for(int j=0; j<cDoF.s.n_rows; j++)
                    {
                        tmpG(tmpDoFv(cDoF.v(i)),tmpDoFs(cDoF.s(j))) += lMat.G(i,j)/mur;
                    }
                }
            }
            tmpSt = tmpSt(tmpDirDoFv,tmpDirDoFv);
            tmpTt = tmpTt(tmpDirDoFv,tmpDirDoFv);
            tmpTt2 = tmpTt2(tmpDirDoFv,tmpDirDoFv);
            tmpSz = tmpSz(tmpDirDoFs,tmpDirDoFs);
            tmpTz = tmpTz(tmpDirDoFs,tmpDirDoFs);
            tmpG = tmpG(tmpDirDoFv,tmpDirDoFs);
            size_t numt = tmpSt.n_rows;
            size_t numz = tmpSz.n_rows;
            size_t numtot = numt + numz;
            arma::cx_mat tmpA(numtot,numtot);
            arma::cx_mat tmpB(numtot,numtot);
            tmpA.fill(0);
            tmpB.fill(0);
            tmpA(arma::span(0,numt-1),arma::span(0,numt-1)) = tmpSt-kk*tmpTt;
            tmpB(arma::span(0,numt-1),arma::span(0,numt-1)) = tmpTt2;
            if(numz>0)
            {
                tmpB(arma::span(numt,numtot-1), arma::span(numt,numtot-1)) = tmpSz-kk*tmpTz;
                tmpB(arma::span(0,numt-1), arma::span(numt,numtot-1)) = tmpG;
                tmpB(arma::span(numt,numtot-1),arma::span(0,numt-1)) = tmpG.st();
            }
            tmpSt.clear();
            tmpTt.clear();
            tmpTt2.clear();
            tmpSz.clear();
            tmpTz.clear();
            tmpG.clear();
            {
                double shift = -kk*maxepsr*maxmur;
                Eigen cEigen(tmpA, tmpB, numt, numz, shift, bc->numModes);
                bc->ModeBeta = cEigen.modeBeta;
                if(opt->verbose)
                {
                    for(int i=0; i<bc->numModes; i++)
                    {
                        std::cout << bc->ModeBeta(i);
                    }
                }
                bc->ModeVec = cEigen.modeVec.rows(0,numt-1)*arma::inv(arma::sqrt(cEigen.modeVec.st()*tmpB*cEigen.modeVec));
                arma::cx_mat coeff(bc->ModeBeta.size(),bc->ModeBeta.size());
                coeff.diag() = arma::sqrt(std::complex<double>(0.0, k0*Const::z0)/bc->ModeBeta);
                //std::cout << coeff;
                bc->ModeVecf = (tmpB * (cEigen.modeVec * coeff *
                                        arma::inv(arma::sqrt(cEigen.modeVec.st()*tmpB*cEigen.modeVec))));
                bc->ModeVecf = bc->ModeVecf.rows(0,numt-1);
                bc->ModeVecDoF = tmpEdges.elem(tmpDirDoFv);
            }
            tmpA.clear();
            tmpB.clear();
            sys->WavePortsNum += bc->numModes;
            sys->WavePortsDoFnum += bc->ModeVec.n_rows;
            sys->WavePortIds = arma::join_cols(sys->WavePortIds,bc->ModeVecDoF);
            if(opt->verbose)
            {
                std::cout << " ";
            }
            logFile << lt.toc() << " s\n";
        }
    }
    MemStat::print(logFile);
    logFile << "Finishing:\n";
    lt.tic();
    if(sys->WavePortsNum > 0)
    {
        if(opt->tfe)
        {
            gmm::row_matrix<std::vector<std::complex<double> > > Meig, MAcoeff;
            gmm::resize(Meig, sys->WavePortsNum, sys->WavePortsDoFnum);
            gmm::resize(MAcoeff, sys->WavePortsNum, sys->WavePortsNum);
            gmm::resize(sys->B, sys->WavePortsNum, sys->WavePortsNum);
            size_t idx = 0;
            size_t jdx = 0;
            for(size_t bcid = 0; bcid < msh->facBC.size(); bcid++)
            {
                BC* bc = &(msh->facBC[bcid]);
                if(bc->type == BC::WavePort)
                {
                    for(size_t i=0; i<bc->numModes; i++)
                    {
                        std::complex<double> sqrtBeta(std::sqrt(std::complex<double>(0.0, k0*Const::z0*opt->power)/bc->ModeBeta(i)));
                        MAcoeff(idx,idx) = std::complex<double>(0.0, k0*Const::z0*opt->power);
                        // j 2 k0 z0
                        sys->B(idx,idx) = std::complex<double>(0.0, 2*k0*Const::z0*opt->power);
                        #pragma omp parallel for
                        for(size_t j=0; j< bc->ModeVec.n_rows; j++)
                        {
                            #pragma omp critical
                            Meig(idx,jdx+j) = sqrtBeta * bc->ModeVec(j,i);
                        }
                        idx++;
                    }
                    jdx += bc->ModeVec.n_rows;
                    for(size_t i=0; i<bc->ModeVecDoF.size(); i++)
                    {
                        bc->ModeVecDoF(i) = sys->InvDoFmapv(bc->ModeVecDoF(i));
                    }
                }
            }
            sys->NonWavePortIds.resize(sys->DoFnum-(sys->WavePortIds.n_rows+sys->DirDoFv.n_rows));
            idx = 0;
            for(size_t i = 0; i<sys->DoFnum; i++)
            {
                if(DoFtoLeave[i] == true)
                {
                    sys->NonWavePortIds(idx++) = i;
                }
            }
            std::vector<size_t> idToKeep;
            for(size_t rgid=sys->DoFlevel[0]; rgid < sys->DoFlevel[1]; rgid++)
            {
                if(DoFtoLeave[rgid] == true)
                {
                    idToKeep.push_back(rgid);
                }
            }
            std::vector<size_t> newDoFlevel;
            idx = 0;
            newDoFlevel.push_back(idx);
            idx = sys->WavePortsNum;
            for(size_t irg=1; irg< sys->DoFlevel.size(); irg++)
            {
                for(size_t rgid=sys->DoFlevel[irg-1]; rgid<sys->DoFlevel[irg]; rgid++)
                {
                    if(DoFtoLeave[rgid] == true)
                    {
                        idx++;
                    }
                }
                newDoFlevel.push_back(idx);
            }
            sys->DoFlevel = newDoFlevel;
            newDoFlevel.clear();
            for(size_t did = 0; did < msh->nDomains; did++)
            {
                EqSys::MatRowType tmpMat(idToKeep.size(),idToKeep.size());
                gmm::copy(gmm::sub_matrix(sys->AFF[did], gmm::sub_index(idToKeep)),tmpMat);
                gmm::swap(sys->AFF[did],tmpMat);
            }
            sys->DoFreal = sys->NonWavePortIds.n_rows + sys->WavePortsNum;
            if(opt->verbose)
            {
                std::cout << "\nSYS DoF = " << sys->DoFreal << "\n";
            }
            logFile << "\tSYS DoF = " << sys->DoFreal << ", ";
            gmm::resize(sys->B, sys->DoFreal, sys->WavePortsNum);
            lt.tic(); /// there
            std::vector<size_t> nnWPids(sys->NonWavePortIds.n_rows);
            std::vector<size_t> WPids(sys->WavePortIds.n_rows);
            #pragma omp parallel for
            for(size_t i=0; i<sys->NonWavePortIds.n_rows; i++)
            {
                nnWPids[i] = sys->NonWavePortIds(i);
                sys->NonWavePortIds(i) = sys->InvDoFmapv(sys->NonWavePortIds(i));
            }
            #pragma omp parallel for
            for(size_t i=0; i<sys->WavePortIds.n_rows; i++)
            {
                WPids[i] = sys->WavePortIds(i);
            }
            sys->NonDirIds.resize(sys->DoFreal);
            idx = 0;
            for(size_t i = 0; i<sys->DoFnum; i++)
            {
                if(DoFtoLeave[i] == true)
                {
                    sys->NonDirIds[idx++] = i;
                }
            }
            for(size_t i = 0; i<sys->DoFreal; i++)
            {
                sys->NonDirIds[i] =  sys->InvDoFmapv(sys->NonDirIds[i]);
            }
            EqSys::MatRowType Anew(sys->DoFreal, sys->DoFreal), Adiag(sys->DoFnum, sys->DoFnum), Afull(sys->DoFnum, sys->DoFnum);
            EqSys::MatRowType Attred1(sys->WavePortsNum,WPids.size()), Attred2(sys->WavePortsNum,WPids.size());
            EqSys::MatRowType Atired1(sys->WavePortsNum,nnWPids.size()), Atired2(sys->WavePortsNum,nnWPids.size());
            // Aii
            gmm::copy(gmm::sub_matrix(sys->A, gmm::sub_index(nnWPids)), gmm::sub_matrix(Anew, gmm::sub_interval(sys->WavePortsNum, nnWPids.size())));
            #pragma omp parallel for
            for(size_t i=0; i<sys->DoFnum; i++)
            {
                Adiag(i,i) = sys->A(i,i);
                sys->A(i,i) *= 0.0;
            }
            gmm::mult(Meig, gmm::sub_matrix(sys->A, gmm::sub_index(WPids)), Attred1);
            gmm::mult(Meig, gmm::sub_matrix(gmm::transposed(sys->A), gmm::sub_index(WPids)), Attred2);
            gmm::add(Attred2, Attred1);
            gmm::mult(Meig, gmm::sub_matrix(Adiag, gmm::sub_index(WPids)), Attred2);
            gmm::add(Attred2, Attred1);
            gmm::mult(Attred1, gmm::transposed(Meig), gmm::sub_matrix(Anew, gmm::sub_interval(0, sys->WavePortsNum)));
            gmm::add(MAcoeff, gmm::sub_matrix(Anew, gmm::sub_interval(0, sys->WavePortsNum)));
            Attred1.clear_mat();
            Attred2.clear_mat();
            for(size_t i=0; i < sys->WavePortsNum; i++)
            {
                for(size_t j=0; j < sys->WavePortsNum; j++)
                {
                    Anew(i,j) *= double(i>=j);
                }
            }
            gmm::mult(Meig, gmm::sub_matrix(sys->A, gmm::sub_index(WPids), gmm::sub_index(nnWPids)), Atired1);
            gmm::mult(Meig, gmm::sub_matrix(gmm::transposed(sys->A), gmm::sub_index(WPids), gmm::sub_index(nnWPids)), Atired2);
            gmm::add(Atired2, Atired1);
            gmm::mult(Meig, gmm::sub_matrix(Adiag, gmm::sub_index(WPids), gmm::sub_index(nnWPids)), Atired2);
            gmm::add(Atired2, Atired1);
            gmm::copy(Atired1, gmm::sub_matrix(Anew, gmm::sub_interval(0,sys->WavePortsNum),gmm::sub_interval(sys->WavePortsNum,nnWPids.size())));
            Atired1.clear_mat();
            Atired2.clear_mat();
            Adiag.clear_mat();
            std::swap(sys->A, Anew);
            Anew.clear_mat();
            Meig.clear_mat();
            MAcoeff.clear_mat();
            nnWPids.clear();
            WPids.clear();
        }
        else
        {
            sys->DoFreal = sys->DoFnum-sys->DirDoFv.n_rows;
            std::cout << "\nSYS DoF = " << sys->DoFreal << "\n";
            logFile << "\tSYS DoF = " << sys->DoFreal << ", ";
            gmm::resize(sys->B, sys->DoFnum, sys->WavePortsNum);
            size_t idx = 0;
            for(size_t bcid = 0; bcid < msh->facBC.size(); bcid++)
            {
                BC* bc = &(msh->facBC[bcid]);
                if(bc->type == BC::WavePort)
                {
                    for(size_t in = 0; in < bc->ModeBeta.size(); in++)
                    {
                        std::complex<double> coeff = bc->ModeBeta(in);
                        #pragma omp parallel for
                        for(size_t fid = 0; fid < bc->Faces.size(); fid++)
                        {
                            arma::uvec adjTet = msh->facAdjTet(bc->Faces(fid));
                            Mtrl* cMtrl = &(msh->tetMtrl[msh->tetLab(adjTet(0))]);
                            std::complex<double> epsr(cMtrl->epsr, cMtrl->CalcEpsr2(sys->freq));
                            double mur = cMtrl->mur;
                            EleMat lMat(opt->pOrd, 2, msh->facGeo(bc->Faces(fid)), quad,
                                        cMtrl, msh->intNode(bc->Faces(fid)));
                            DoF cDoF(prj, 2, bc->Faces(fid));
                            cDoF.v = sys->DoFmapv.elem(cDoF.v);
                            #pragma omp critical
                            for(int i=0; i<cDoF.v.n_rows; i++)
                            {
                                for(int j=0; j<cDoF.v.n_rows; j++)
                                {
                                    if(cDoF.v(i)<=cDoF.v(j))
                                    {
                                        sys->A(cDoF.v(i),cDoF.v(j)) += lMat.Tt(i,j)*coeff;
                                        if((cDoF.v(i)<sys->DoFlevel[1]) && (cDoF.v(j) < sys->DoFlevel[1]))
                                        {
                                            sys->AFF[msh->tetDom(adjTet(0))](cDoF.v(i),cDoF.v(j)) +=
                                                lMat.Tt(i,j)*coeff;
                                        }
                                    }
                                }
                            }
                        }
                        for(size_t i=0; i<bc->ModeVecDoF.size(); i++)
                        {
                            sys->B(bc->ModeVecDoF(i),idx) = 2.0 * coeff * bc->ModeVecf(i,in);
                        }
                        idx++;
                    }
                }
            }
            sys->NonDirIds.resize(sys->DoFreal);
            idx = 0;
            for(size_t i = 0; i<sys->DoFnum; i++)
            {
                if(DoFtoLeave[i] == true)
                {
                    sys->NonDirIds[idx++] = i;
                }
            }
            EqSys::MatRowType Anew(sys->DoFreal, sys->DoFreal);
            EqSys::MatColType Bnew(sys->DoFreal, sys->WavePortsNum);
            gmm::copy(gmm::sub_matrix(sys->A, gmm::sub_index(sys->NonDirIds)),Anew);
            //gmm::sub_matrix(Anew, gmm::sub_interval(0, sys->DoFreal)));
            gmm::copy(gmm::sub_matrix(sys->B, gmm::sub_index(sys->NonDirIds), gmm::sub_interval(0,sys->WavePortsNum)),Bnew);
            //gmm::sub_matrix(Anew, gmm::sub_interval(0, sys->DoFreal)));
            std::swap(sys->A, Anew);
            std::swap(sys->B, Bnew);
            Anew.clear_mat();
            Bnew.clear_mat();
            std::vector<size_t> idToKeep;
            for(size_t rgid=sys->DoFlevel[0]; rgid < sys->DoFlevel[1]; rgid++)
            {
                if(DoFtoLeave[rgid] == true)
                {
                    idToKeep.push_back(rgid);
                }
            }
            std::vector<size_t> newDoFlevel;
            idx = 0;
            newDoFlevel.push_back(idx);
            //idx = sys->WavePortsNum;
            for(size_t irg=1; irg< sys->DoFlevel.size(); irg++)
            {
                for(size_t rgid=sys->DoFlevel[irg-1]; rgid<sys->DoFlevel[irg]; rgid++)
                {
                    if(DoFtoLeave[rgid] == true)
                    {
                        idx++;
                    }
                }
                newDoFlevel.push_back(idx);
            }
            sys->DoFlevel = newDoFlevel;
            newDoFlevel.clear();
            for(size_t did = 0; did < msh->nDomains; did++)
            {
                EqSys::MatRowType tmpMat(idToKeep.size(),idToKeep.size());
                gmm::copy(gmm::sub_matrix(sys->AFF[did], gmm::sub_index(idToKeep)),tmpMat);
                gmm::swap(sys->AFF[did],tmpMat);
                //std::cout << sys->DoFlevel[1]-sys->WavePortsNum << "\n";
                //std::cout << sys->AFF[did] << "\n";
            }
        }
    }
    else if(opt->einc)
    {
        arma::vec kEinc(3), polEinc(3);
        kEinc(0) = opt->k[0];
        kEinc(1) = opt->k[1];
        kEinc(2) = opt->k[2];
        polEinc(0) = opt->E[0];
        polEinc(1) = opt->E[1];
        polEinc(2) = opt->E[2];
        kEinc *= k0;
        polEinc /= std::sqrt(2.0); // Vrms
        sys->DoFreal = sys->DoFnum;
        if(opt->verbose)
        {
            std::cout << "\nSYS DoF = " << sys->DoFreal << "\n";
        }
        logFile << "\tSYS DoF = " << sys->DoFreal << ", ";
        sys->Sol.clear();
        sys->Sol.resize(sys->DoFreal,1);
        sys->Sol.fill(0);
        gmm::resize(sys->B, sys->DoFnum, 1);
        for(size_t bcid = 0; bcid < msh->facBC.size(); bcid++)
        {
            BC* bc = &(msh->facBC[bcid]);
            if(bc->type == BC::Radiation)
            {
                #pragma omp parallel for
                for(size_t fid = 0; fid < bc->Faces.size(); fid++)
                {
                    arma::uvec adjTet = msh->facAdjTet(bc->Faces(fid));
                    double epsr = msh->tetMtrl[msh->tetLab(adjTet(0))].epsr;
                    double mur = msh->tetMtrl[msh->tetLab(adjTet(0))].mur;
                    EleMat lMat(opt->pOrd, 2, msh->facGeo(bc->Faces(fid)), quad,
                                &(msh->tetMtrl[msh->tetLab(adjTet(0))]),
                                msh->intNode(bc->Faces(fid)), kEinc, polEinc);
                    DoF cDoF(prj, 2, bc->Faces(fid));
                    #pragma omp critical
                    for(int i=0; i<cDoF.v.n_rows; i++)
                    {
                        sys->B(cDoF.v(i),0) += lMat.f(i) * std::complex<double>(0.0,k0);
                    }
                }
            }
        }
        std::vector<size_t> DirIds(sys->DirDoFv.size());
        #pragma omp parallel for
        for(size_t i=0; i<sys->DirDoFv.size(); i++)
        {
            DirIds[i] = sys->DirDoFv(i);
        }
        gmm::clear(gmm::sub_matrix(sys->A, gmm::sub_index(DirIds), gmm::sub_interval(0, sys->DoFreal)));
        gmm::clear(gmm::sub_matrix(sys->A, gmm::sub_interval(0, sys->DoFreal), gmm::sub_index(DirIds)));
        #pragma omp parallel for
        for(size_t i=0; i<DirIds.size(); i++)
        {
            sys->A(DirIds[i],DirIds[i]) = 1.0;
        }
    }
    sys->SymmFlag = 2;
    logFile << " " << lt.toc() << " s\n";
    logFile << "+" << tt.toc() << " s\n";
}
AssLinSchur::~AssLinSchur()
{
    //dtor
}
