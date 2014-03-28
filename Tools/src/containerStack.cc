#include "../interface/containerStack.h"
#include <algorithm>
#include <vector>

namespace ztop{

std::vector<ztop::containerStack*> containerStack::cs_list;
bool containerStack::cs_makelist=false;
bool containerStack::batchmode=true;
bool containerStack::debug=false;
bool containerStack::addErrorsStrict=true;

containerStack::containerStack() : name_(""), dataleg_("data"), mode(notset){
    if(cs_makelist)cs_list.push_back(this);
}
containerStack::containerStack(TString name) : name_(name), dataleg_("data"), mode(notset) {
    mode=notset;
    if(cs_makelist) cs_list.push_back(this);
}
containerStack::~containerStack(){
    for(unsigned int i=0;i<cs_list.size();i++){
        if(cs_list[i] == this) cs_list.erase(cs_list.begin()+i);
        break;
    }
}
void containerStack::push_back(ztop::container1D cont, TString legend, int color, double norm, int legord){
    if(mode==notset)
        mode=dim1;
    else if(mode!=dim1){
        std::cout << "containerStack::push_back: trying to add 1D container to non-1D stack!" << std::endl;
        return;
    }
    if(cont.hasTag()){
        if(debug) std::cout << "containerStack::push_back: adding tag" << std::endl;
        addTagsFrom(&cont);
    }

    bool wasthere=false;
    for(unsigned int i=0;i<legends_.size();i++){
        if(legend == legends_[i]){
            if(containerStack::debug)
                std::cout << "containerStack::push_back(1D): found same legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
            containers_[i] = containers_[i] * norms_[i] + cont * norm;
            containers_[i].setName(getName());
            norms_[i]=1;
            legorder_[i]=legord;
            wasthere=true;
            break;
        }
    }
    if(!wasthere){
        if(containerStack::debug)
            std::cout << "containerStack::push_back (1D): new legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
        cont.setName(getName());
        containers_.push_back(cont*norm);
        legends_.push_back(legend);
        colors_.push_back(color);
        norms_.push_back(1);
        legorder_.push_back(legord);
    }
}

void containerStack::push_back(ztop::container2D cont, TString legend, int color, double norm, int legord){
    if(mode==notset)
        mode=dim2;
    else if(mode!=dim2){
        std::cout << "containerStack::push_back(2D): trying to add 2D container to non-2D stack!" << std::endl;
        return;
    }
    bool wasthere=false;
    if(cont.hasTag()){
        addTagsFrom(&cont);
    }
    for(unsigned int i=0;i<legends_.size();i++){
        if(legend == legends_[i]){
            if(containerStack::debug)
                std::cout << "containerStack::push_back(2D): found same legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
            containers2D_[i] = containers2D_[i] * norms_[i] + cont * norm;
            containers2D_[i].setName(getName());
            norms_[i]=1;
            legorder_[i]=legord;
            wasthere=true;
            break;
        }
    }
    if(!wasthere){
        if(containerStack::debug)
            std::cout << "containerStack::push_back (2D): new legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
        cont.setName(getName());
        containers2D_.push_back(cont*norm);
        legends_.push_back(legend);
        colors_.push_back(color);
        norms_.push_back(1);
        legorder_.push_back(legord);
    }
}
void containerStack::push_back(ztop::container1DUnfold cont, TString legend, int color, double norm, int legord){
    if(mode==notset)
        mode=unfolddim1;
    else if(mode!=unfolddim1){
        std::cout << "containerStack::push_back: trying to add 1DUnfold container to non-1DUnfold stack!" << std::endl;
        return;
    }
    if(cont.hasTag()){
        addTagsFrom(&cont);
    }
    container1D cont1d=cont.getControlPlot();
    bool wasthere=false;
    for(unsigned int i=0;i<legends_.size();i++){
        if(legend == legends_[i]){
            if(containerStack::debug)
                std::cout << "containerStack::push_back(1DUF): found same legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
            containers1DUnfold_[i] = containers1DUnfold_[i] * norms_[i]  + cont * norm;
            containers1DUnfold_[i].setName(getName());
            containers_[i] = containers_[i] * norms_[i] + cont1d * norm;
            containers_[i].setName(getName());
            norms_[i]=1;
            legorder_[i]=legord;
            wasthere=true;
            break;
        }
    }
    if(!wasthere){
        if(containerStack::debug)
            std::cout << "containerStack::push_back (1DUF): new legend ("<<  legend <<"), adding " << cont.getName() << std::endl;
        cont.setName(getName());
        containers1DUnfold_.push_back(cont * norm);
        cont1d.setName(getName());
        containers_.push_back(cont1d * norm);
        legends_.push_back(legend);
        colors_.push_back(color);
        norms_.push_back(1);
        legorder_.push_back(legord);
    }

}

void containerStack::setLegendOrder(const TString &leg, const size_t& no){
    for(size_t i=0;i<legends_.size();i++){
        if(legends_.at(i)==leg){
            if(legorder_.size()>i){
                legorder_.at(i)=no;
                return;
            }
            else{
                std::cout << "containerStack::setLegendOrder: size doesn't match" <<std::endl;
                return;
            }
        }
    }
}

void containerStack::removeContribution(TString legendname){
    bool found=false;
    std::vector<TString>::iterator leg=legends_.begin();
    std::vector<int>::iterator col=colors_.begin();
    std::vector<int>::iterator lego=legorder_.begin();
    std::vector<double>::iterator norm=norms_.begin();
    size_t rmpos=0;
    for(size_t i=0;i<legends_.size();i++){
        if(*leg == legendname){
            legends_.erase(leg);
            colors_.erase(col);
            norms_.erase(norm);
            legorder_.erase(lego);
            rmpos=i;
            found=true;
            break;
        }
        ++leg;++col;++norm;++lego;

    }
    if(mode==dim1 || mode==unfolddim1)
        containers_.erase(containers_.begin()+rmpos);
    else if(mode==dim2)
        containers2D_.erase(containers2D_.begin()+rmpos);
    else if(mode==unfolddim1)
        containers1DUnfold_.erase(containers1DUnfold_.begin()+rmpos);

    if(!found) std::cout << "containerStack::removeContribution: " << legendname << " not found." <<std::endl;

}

void containerStack::mergeSameLegends(){
    //redundant
}
int containerStack::getContributionIdx(TString legname) const{
    for(size_t i=0;i<legends_.size();i++){
        if(legname == legends_.at(i))
            return i;
    }
    std::cout << "containerStack::getContributionIdx: " << legname << " not found!" <<std::endl;
    return -1;
}

ztop::container1D containerStack::getContribution(TString contr) const{
    ztop::container1D out;
    int idx=getContributionIdx(contr);
    if(idx<0 || mode != dim1)
        return out;
    out=containers_.at(idx);
    out*= norms_.at(idx);
    return out;
}
ztop::container2D containerStack::getContribution2D(TString contr)const{
    ztop::container2D out;
    int idx=getContributionIdx(contr);
    if(idx<0 || mode != dim2)
        return out;
    out=containers2D_.at(idx);
    out*= norms_.at(idx);
    return out;
}
ztop::container1DUnfold containerStack::getContribution1DUnfold(TString contr)const{
    ztop::container1DUnfold out;
    int idx=getContributionIdx(contr);
    if(idx<0 || mode != unfolddim1)
        return out;
    out=containers1DUnfold_.at(idx);
    out*= norms_.at(idx);
    return out;
}
ztop::container1D containerStack::getContributionsBut(TString contr)const{
    ztop::container1D out;
    if(mode != dim1)
        return out;
    out=containers_.at(0);
    out.clear();
    int i=0;
    for(std::vector<TString>::const_iterator name=legends_.begin();name<legends_.end();++name){
        if(contr != *name){
            container1D temp=containers_[i];
            out=out+temp * norms_[i];
        }
        i++;
    }
    return out;
}
ztop::container2D containerStack::getContributions2DBut(TString contr) const{
    ztop::container2D out;
    if(mode != dim2)
        return out;
    out=containers2D_.at(0);
    out.clear();
    int i=0;
    for(std::vector<TString>::const_iterator name=legends_.begin();name<legends_.end();++name){
        if(contr != *name){
            container2D temp=containers2D_[i];
            out=out+temp * norms_[i];
        }
        i++;
    }
    return out;
}
ztop::container1DUnfold containerStack::getContributions1DUnfoldBut(TString contr) const{
    ztop::container1DUnfold out;
    if(mode != dim2)
        return out;
    out=containers1DUnfold_.at(0);
    out.clear();
    int i=0;
    for(std::vector<TString>::const_iterator name=legends_.begin();name<legends_.end();++name){
        if(contr != *name){
            container1DUnfold temp=containers1DUnfold_[i];
            out=out+temp * norms_[i];
        }
        i++;
    }
    return out;
}
ztop::container1D containerStack::getContributionsBut(std::vector<TString> contr) const{
    ztop::container1D out;
    if(mode!=dim1)
        return out;
    out=containers_.at(0);
    out.clear();
    for(unsigned int i=0;i<legends_.size();i++){
        bool get=true;
        for(std::vector<TString>::const_iterator name=contr.begin();name<contr.end();++name){
            if(*name == legends_[i]){
                get=false;
                break;
            }
        }
        if(get){
            container1D temp=containers_[i];
            out +=  temp* norms_[i];
        }
    }
    return out;
}
ztop::container2D containerStack::getContributions2DBut(std::vector<TString> contr) const{
    ztop::container2D out;
    if(mode!=dim2)
        return out;
    out=containers2D_.at(0);
    out.clear();
    for(unsigned int i=0;i<legends_.size();i++){
        bool get=true;
        for(std::vector<TString>::const_iterator name=contr.begin();name<contr.end();++name){
            if(*name == legends_[i]){
                get=false;
                break;
            }
        }
        if(get){
            container2D temp=containers2D_[i] ;
            out = out+ temp* norms_[i];
        }
    }
    return out;
}
ztop::container1DUnfold containerStack::getContributions1DUnfoldBut(std::vector<TString> contr) const{
    ztop::container1DUnfold out;
    if(mode!=unfolddim1)
        return out;
    out=containers1DUnfold_.at(0);
    out.clear();
    for(unsigned int i=0;i<legends_.size();i++){
        bool get=true;
        for(std::vector<TString>::const_iterator name=contr.begin();name<contr.end();++name){
            if(*name == legends_[i]){
                get=false;
                break;
            }
        }
        if(get){
            container1DUnfold temp=containers1DUnfold_[i] ;
            out = out+ temp* norms_[i];
        }
    }
    return out;
}

void containerStack::multiplyNorm(TString legendentry, double multi){
    int i=0;
    for(std::vector<TString>::iterator name=legends_.begin();name<legends_.end();++name){
        if(legendentry == *name){
            if(dim1){
                containers_.at(i)*=multi;
            }
            else if(dim2){
                containers2D_.at(i)*=multi;
            }
            else if(unfolddim1){
                containers_.at(i)*=multi;
                containers1DUnfold_.at(i)*=multi;
            }
            break;
        }
        i++;
    }

}

void containerStack::multiplyAllMCNorms(double multiplier){
    for(unsigned int i=0;i<legends_.size();i++){
        if(legends_.at(i)!=dataleg_){
            if(dim1){
                containers_.at(i)*=multiplier;
            }
            else if(dim2){
                containers2D_.at(i)*=multiplier;
            }
            else if(unfolddim1){
                containers_.at(i)*=multiplier;
                containers1DUnfold_.at(i)*=multiplier;
            }
        }
    }
}

void containerStack::addGlobalRelMCError(TString sysname,double error){
    for(unsigned int i=0;i<containers_.size();i++){
        if(legends_[i]!=dataleg_) containers_[i].addGlobalRelError(sysname,error);
    }
    for(unsigned int i=0;i<containers2D_.size();i++){
        if(legends_[i]!=dataleg_) containers2D_[i].addGlobalRelError(sysname,error);
    }
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        if(legends_[i]!=dataleg_) containers1DUnfold_[i].addGlobalRelError(sysname,error);
    }
}
void containerStack::addMCErrorStack(const TString &sysname,const containerStack &errorstack){
    addErrorStack( sysname, errorstack);
}

void containerStack::addErrorStack(const TString & sysname, containerStack errorstack){
    if(mode!=errorstack.mode){
        std::cout << "containerStack::addErrorStack: stacks must have same type" << std::endl;
        return;
    }
    if(debug){
        std::cout << "containerStack::addErrorStack: " << name_ << std::endl;
        std::cout << "legends lhs: ";
        for(size_t i=0;i<legends_.size();i++)
            std::cout << legends_.at(i) << " ";
        std::cout << std::endl;
        std::cout << "legends rhs: ";
        for(size_t i=0;i<errorstack.legends_.size();i++)
            std::cout << errorstack.legends_.at(i) << " ";
        std::cout << std::endl;
    }

    for(unsigned int i=0; i<size();i++){
        /*bool found=false;
		for(unsigned int j=0;j<errorstack.size();j++){
			if(legends_[i] == errorstack.legends_[j]){ //costs performance!
         */
        std::vector<TString>::iterator it=std::find(errorstack.legends_.begin(),errorstack.legends_.end(),legends_[i]);
        if(it!=errorstack.legends_.end()){
            size_t j=it-errorstack.legends_.begin();
            //found=true;
            if(mode==dim1 || mode==unfolddim1){
                errorstack.containers_[j] = errorstack.containers_[j] * errorstack.norms_[j]; //normalize (in case there is any remultiplication done or something)
                errorstack.norms_[j]=1;
                containers_[i] = containers_[i] * norms_[i];
                norms_[i]=1;
                containers_[i].addErrorContainer(sysname,errorstack.containers_[j]);
            }
            if(mode==dim2){
                errorstack.containers2D_[j] = errorstack.containers2D_[j] * errorstack.norms_[j]; //normalize (in case there is any remultiplication done or something)
                errorstack.norms_[j]=1;
                containers2D_[i] = containers2D_[i] * norms_[i];
                norms_[i]=1;
                containers2D_[i].addErrorContainer(sysname,errorstack.containers2D_[j]);
            }
            if(mode==unfolddim1){
                errorstack.containers1DUnfold_[j] = errorstack.containers1DUnfold_[j] * errorstack.norms_[j]; //normalize (in case there is any remultiplication done or something)
                errorstack.norms_[j]=1;
                containers1DUnfold_[i] = containers1DUnfold_[i] * norms_[i];
                norms_[i]=1;
                containers1DUnfold_[i].addErrorContainer(sysname,errorstack.containers1DUnfold_[j]);
            }
        }//legfound
        /*} */
        else{ //if(!found)
            if(addErrorsStrict){
                throw std::runtime_error("containerStack::addErrorStack: legend in one stack not found. To allow this, switch containerStack::addErrorsStrict to false");
            }
            std::cout << "containerStack::addErrorStack: "<< name_ << " legend " << legends_[i] << " not found. adding 0 error!" << std::endl;
            if(mode==dim1 || mode==unfolddim1){
                containers_[i].addErrorContainer(sysname,containers_[i]);
            }
            if(mode==dim2){
                containers2D_[i].addErrorContainer(sysname,containers2D_[i]);
            }
            if(mode==unfolddim1){
                containers1DUnfold_[i].addErrorContainer(sysname,containers1DUnfold_[i]);
            }

        }
    }
}

void containerStack::getRelSystematicsFrom(ztop::containerStack stack){
    for(std::vector<ztop::container1D>::iterator cont=stack.containers_.begin();cont<stack.containers_.end();++cont){
        TString name=cont->getName();
        for(unsigned int i=0;i<containers_.size();i++){
            if(containers_[i].getName() == name){
                containers_[i].getRelSystematicsFrom(*cont);
                break;
            }
        }
    }
    for(std::vector<ztop::container2D>::iterator cont=stack.containers2D_.begin();cont<stack.containers2D_.end();++cont){
        TString name=cont->getName();
        for(unsigned int i=0;i<containers2D_.size();i++){
            if(containers2D_[i].getName() == name){
                containers2D_[i].getRelSystematicsFrom(*cont);
                break;
            }
        }
    }

    for(std::vector<ztop::container1DUnfold>::iterator cont=stack.containers1DUnfold_.begin();cont<stack.containers1DUnfold_.end();++cont){
        TString name=cont->getName();
        for(unsigned int i=0;i<containers1DUnfold_.size();i++){
            if(containers1DUnfold_[i].getName() == name){
                containers1DUnfold_[i].getRelSystematicsFrom(*cont);
                break;
            }
        }
    }
}

void containerStack::removeError(TString sysname){
    for(unsigned int i=0; i<containers_.size();i++){
        containers_[i].removeError(sysname);
    }
    for(unsigned int i=0; i<containers2D_.size();i++){
        containers2D_[i].removeError(sysname);
    }
    for(unsigned int i=0; i<containers1DUnfold_.size();i++){
        containers1DUnfold_[i].removeError(sysname);
    }
}

void containerStack::renameSyst(TString old, TString New){
    for(unsigned int i=0; i<containers_.size();i++){
        containers_[i].renameSyst(old,New);
    }
    for(unsigned int i=0; i<containers2D_.size();i++){
        containers2D_[i].renameSyst(old,New);
    }
    for(unsigned int i=0; i<containers1DUnfold_.size();i++){
        containers1DUnfold_[i].renameSyst(old,New);
    }
}
ztop::container1D & containerStack::getContainer(TString name){

    bool found=false;
    for(unsigned int i=0;i<containers_.size();i++){
        if(containers_[i].getName() == name){
            containers_[i] = containers_[i] * norms_[i];
            norms_[i] = 1.;
            found=true;
            return containers_[i];
        }
    }
    if(!found) std::cout << "containerStack::getContainer: container with name " << name << " not found, returning first container! all names are formatted: c_legendentry" << std::endl;
    return containers_.at(0);
}
ztop::container1D  containerStack::getContainer(TString name)const{
    bool found=false;
    container1D out;
    for(unsigned int i=0;i<containers_.size();i++){
        if(containers_[i].getName() == name){
            out=containers_[i];
            out *= norms_[i];
            found=true;
            return out;
        }
    }
    if(!found) std::cout << "containerStack::getContainer: container with name " << name << " not found, returning first container! all names are formatted: c_legendentry" << std::endl;
    return containers_.at(0);
}

ztop::container2D & containerStack::getContainer2D(TString name){
    bool found=false;
    for(unsigned int i=0;i<containers2D_.size();i++){
        if(containers2D_[i].getName() == name){
            containers2D_[i] = containers2D_[i] * norms_[i];
            norms_[i] = 1.;
            found=true;
            return containers2D_[i];
        }
    }
    if(!found) std::cout << "containerStack::getContainer2D: container with name " << name << " not found, returning first container! all names are formatted: c_legendentry" << std::endl;
    return containers2D_.at(0);
}
ztop::container2D  containerStack::getContainer2D(TString name)const{
    bool found=false;
    container2D out;
    for(unsigned int i=0;i<containers2D_.size();i++){
        if(containers2D_[i].getName() == name){
            out=containers2D_[i];
            out *= norms_[i];
            found=true;
            return out;
        }
    }
    if(!found) std::cout << "containerStack::getContainer2D: container with name " << name << " not found, returning first container! all names are formatted: c_legendentry" << std::endl;
    return containers2D_.at(0);
}
ztop::container1DUnfold & containerStack::getContainer1DUnfold(TString name){
    bool found=false;
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        if(containers1DUnfold_[i].getName() == name){
            containers1DUnfold_[i] = containers1DUnfold_[i] * norms_[i];
            norms_[i] = 1.;
            found=true;
            return containers1DUnfold_[i];
        }
    }
    if(!found) std::cout << "containerStack::getContainer1DUnfold: container with name " << name << " not found, returning first container! all names are formatted: c_legendentry" << std::endl;
    return containers1DUnfold_.at(0);
}
ztop::container1DUnfold  containerStack::getContainer1DUnfold(TString name)const{
    bool found=false;
    container1DUnfold out;
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        if(containers1DUnfold_[i].getName() == name){
            out=containers1DUnfold_[i];
            out *= norms_[i];
            found=true;
            return out;
        }
    }
    if(!found) std::cout << "containerStack::getContainer1DUnfold: container with name " << name << " not found, returning first container! all names are formatted: c_legendentry" << std::endl;
    return containers1DUnfold_.at(0);
}
ztop::container1D containerStack::getFullMCContainer()const{
    container1D out;
    if(containers_.size()<1)
        return out;
    out=containers_[0];
    out.clear();
    for(unsigned int i=0;i<containers_.size();i++){
        if(legends_[i] != dataleg_) {
            container1D temp=containers_[i];
            out = out + temp*norms_[i];
        }
    }
    return out;
}
ztop::container2D containerStack::getFullMCContainer2D()const{
    container2D out;
    if(containers2D_.size()<1)
        return out;
    out=containers2D_[0];
    out.clear();
    for(unsigned int i=0;i<containers2D_.size();i++){
        if(legends_[i] != dataleg_){
            container2D temp= containers2D_[i];
            out = out + temp*norms_[i];
        }
    }
    return out;
}
ztop::container1DUnfold containerStack::getFullMCContainer1DUnfold()const {
    container1DUnfold out;
    if(containers1DUnfold_.size()<1)
        return out;
    out=containers1DUnfold_[0];
    out.clear();
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        if(legends_[i] != dataleg_){
            container1DUnfold temp=containers1DUnfold_[i];
            out = out + temp*norms_[i];
        }
    }
    return out;
}

THStack * containerStack::makeTHStack(TString stackname){
    if(!checkDrawDimension()){
        std::cout << "containerStack::makeTHStack: only available for 1dim stacks!" <<std::endl;
        return 0;
    }
    if(debug)
        std::cout << "containerStack::makeTHStack: ..." <<std::endl;
    TH1::AddDirectory(false);
    if(stackname == "") stackname = name_+"_s";
    THStack *tstack = addObject(new THStack(stackname,stackname));

    std::vector<size_t> sorted=sortEntries(false);
    if(debug)
        std::cout << "containerStack::makeTHStack: sorted entries" <<std::endl;

    if(debug)
        std::cout << "containerStack::makeTHStack: size="<<containers_.size() <<std::endl;
    for(unsigned int it=0;it<size();it++){
        if(debug)
            std::cout << "containerStack::makeTHStack: get "<< it;
        size_t i=sorted.at(it);
        if(getLegend(i) != dataleg_){
            if(debug)
                std::cout <<"->"<<i << ": " << getLegend(i)<< std::endl;
            container1D tempcont = getContainer(i);

            tempcont = tempcont * getNorm(i);
            TH1D * htemp=(tempcont.getTH1D(getLegend(i)+" "+getName()+"_stack_h"));
            if(!htemp)
                continue;
            TH1D * h = addObject((TH1D*)htemp->Clone());
            delete htemp;
            h->SetFillColor(getColor(i));
            for(int bin = 1 ; bin < h->GetNbinsX()+1;bin++){
                h->SetBinError(bin,0);
            }

            tstack->Add(h);

        }
    }
    if(debug)
        std::cout << "containerStack::makeTHStack: stack produced" <<std::endl;
    return  tstack;
}
TLegend * containerStack::makeTLegend(bool inverse){
    if(!checkDrawDimension()){
        std::cout << "containerStack::makeTLegend: only available for 1dim stacks!" <<std::endl;
        return 0;
    }
    if(debug)
        std::cout << "containerStack::makeTLegend: drawing..." <<std::endl;
    TLegend *leg = addObject(new TLegend((Double_t)0.65,(Double_t)0.50,(Double_t)0.95,(Double_t)0.90));
    leg->Clear();
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);

    std::vector<size_t> sorted=sortEntries(inverse);

    for(unsigned int it=0;it<size();it++){
        size_t i=sorted.at(it);
        container1D tempcont = containers_[i];
        tempcont = tempcont * getNorm(i);
        TH1D * h = addObject((TH1D*)tempcont.getTH1D(getLegend(i)+" "+getName()+"_leg")->Clone());
        h->SetFillColor(getColor(i));
        if(getLegend(i) != dataleg_) leg->AddEntry(h,getLegend(i),"f");
        else leg->AddEntry(h,getLegend(i),"ep");
        //  delete h;
    }

    return leg;
}
void containerStack::drawControlPlot(TString name, bool drawxaxislabels, double resizelabels) {
    if(!checkDrawDimension()){
        std::cout << "containerStack::drawControlPlot: only available for 1dim(unfold) stacks!" <<std::endl;
        return;
    }
    if(debug)
        std::cout << "containerStack::drawControlPlot: drawing " <<name_ <<std::endl;
    TH1::AddDirectory(false);
    if(name=="") name=name_;
    int dataentry=0;
    bool gotdentry=false;
    bool gotuf=false;
    bool gotof=false;
    for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
        if(getLegend(i) == dataleg_){
            dataentry=i;
            gotdentry=true;
            if(gotof && gotuf) break;
        }
        if(containers_[i].getOverflow() < -0.9){
            gotof=true;
            if(gotdentry && gotuf) break;
        }
        if(containers_[i].getUnderflow() < -0.9){
            gotuf=true;
            if(gotdentry && gotof) break;
        }
    }
    float multiplier=1;
    double ylow,yhigh,xlow,xhigh;
    if(gPad){
        gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
        multiplier = (float)1/(yhigh-ylow);
    }
    containers_[dataentry].setLabelSize(resizelabels * multiplier);
    TGraphAsymmErrors * g = addObject(containers_[dataentry].getTGraph(name));
    TH1D * h =addObject(containers_[dataentry].getTH1D(name+"_h")); // needed to be able to set log scale etc.

    float xmax=containers_[dataentry].getXMax();
    float xmin=containers_[dataentry].getXMin();
    h->Draw("AXIS");
    if(!drawxaxislabels){
        h->GetXaxis()->SetLabelSize(0);
    }
    else {
        if(gPad){
            gPad->SetLeftMargin(0.15);
            gPad->SetBottomMargin(0.15);
        }
    }
    g->Draw("P");
    makeTHStack(name)->Draw("same");
    makeMCErrors()->Draw("2");
    makeTLegend()->Draw("same");
    g->Draw("e1,P,same");
    if(gPad) gPad->RedrawAxis();

    if(gPad && drawxaxislabels && containers_[dataentry].getNBins() >0){
        float yrange=fabs(gPad->GetUymax()-gPad->GetUymin());
        float xrange = fabs(xmax-xmin);
        if(gotuf){
            TLatex * la = addObject(new TLatex(containers_[dataentry].getBinCenter(1)-xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
            la->Draw("same");
        }
        if(gotof){
            TLatex * la2 = addObject(new TLatex(containers_[dataentry].getBinCenter(containers_[dataentry].getNBins())+xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
            la2->Draw("same");
        }
    }

}

TGraphAsymmErrors * containerStack::makeMCErrors(){
    TGraphAsymmErrors * g =addObject(getFullMCContainer().getTGraph());
    g->SetFillStyle(3005);
    return g;
}

void containerStack::drawRatioPlot(TString name,double resizelabels){
    if(!checkDrawDimension()){
        std::cout << "containerStack::drawRatioPlot: only available for 1dim stacks!" <<std::endl;
        return;
    }
    if(debug)
        std::cout << "containerStack::drawRatioPlot: drawing..." <<std::endl;
    TH1::AddDirectory(false);
    //prepare container
    if(name=="") name=name_;
    int dataentry=0;
    bool gotdentry=false;
    bool gotuf=false;
    bool gotof=false;
    for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
        if(getLegend(i) == dataleg_){
            dataentry=i;
            gotdentry=true;
            if(gotof && gotuf) break;
        }
        if(containers_[i].getOverflow() < -0.9){
            gotof=true;
            if(gotdentry && gotuf) break;
        }
        if(containers_[i].getUnderflow() < -0.9){
            gotuf=true;
            if(gotdentry && gotof) break;
        }
    }
    container1D data = containers_[dataentry];
    data.setShowWarnings(false);
    container1D mc = getFullMCContainer();
    mc.setShowWarnings(false);
    //data.setDivideBinomial(false);
    //mc.setDivideBinomial(false);
    /*
	container1D ratio=data;
	ratio.clear();
	ratio.setShowWarnings(false);
	container1D relmcerrstat=data;
	relmcerrstat.clear();
	container1D relmcerrsys=relmcerrstat;
	container1D mccopy=mc;
	for(size_t i=0; i<ratio.getNBins()+1; i++){ //includes underflow right now, doesn't matter
		relmcerrstat.setBinContent(i,1.);
		double errfullup=mc.getBinErrorUp(i)/mc.getBinContent(i);
		double errfulldown=mc.getBinErrorDown(i) / mc.getBinContent(i);
		double errstatup=mc.getBinErrorUp(i,true)/mc.getBinContent(i);
		double errstatdown=mc.getBinErrorDown(i,true) / mc.getBinContent(i);
		double errsysup=sqrt(errfullup*errfullup - errstatup*errstatup);
		double errsysdown=sqrt(errfulldown*errfulldown - errstatdown*errstatdown);
		if(mc.getBinContent(i) ==0){
			errfullup=0;errfulldown=0;errsysup=0;errsysdown=0;
		}
		relmcerrstat.setBinErrorUp(i,errfullup);
		relmcerrstat.setBinErrorDown(i,errfulldown);
		relmcerrsys.setBinContent(i,1.);
		relmcerrsys.setBinErrorUp(i,errsysup);
		relmcerrsys.setBinErrorDown(i,errsysdown);
		//set mc error to zero for the ratio plot
		mccopy.setBinErrorUp(i,0);
		mccopy.setBinErrorDown(i,0);
	}
	bool temp=histoContent::divideStatCorrelated;
	histoContent::divideStatCorrelated =false;
	ratio = data / mccopy;
	histoContent::divideStatCorrelated =temp;
     */
    container1D ratio=data;
    ratio.normalizeToContainer(mc);
    mc=mc.getRelErrorsContainer();

    float multiplier=1;
    double ylow,yhigh,xlow,xhigh;
    if(gPad){
        gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
        multiplier = (float)1/(yhigh-ylow);
    }
    //   multiplier = multiplier * resizelabels;
    ratio.setLabelSize(multiplier * resizelabels);

    TGraphAsymmErrors * gratio = addObject(ratio.getTGraph(name,false,false,true)); //don't divide by binwidth, no x errors
    // rescale axis titles etc.
    TH1D * h = addObject(ratio.getTH1D(name+"_h_r",false)); //no bw div
    h->GetYaxis()->SetTitle("data/MC");
    h->GetYaxis()->SetRangeUser(0.5,1.5);
    h->GetYaxis()->SetNdivisions(505);
    h->GetXaxis()->SetTickLength(0.03 * multiplier);
    h->Draw("AXIS");
    gratio->Draw("P");/*
	TGraphAsymmErrors * gmcerr = relmcerrstat.getTGraph(name+"_relerrstat",false,false,false);
	TGraphAsymmErrors * gmcerrsys = relmcerrsys.getTGraph(name+"_relerrsys",false,false,false); */
    TGraphAsymmErrors * gmcerr = addObject(mc.getTGraph(name+"_relerrstat",false,false,false));
    TGraphAsymmErrors * gmcerrsys = addObject(mc.getTGraph(name+"_relerrsys",false,false,false,true));

    gmcerr->SetFillStyle(3005);
    gmcerrsys->SetFillStyle(1001);
    gmcerrsys->SetFillColor(kCyan-10);
    gmcerrsys->Draw("2,same");
    gmcerr->Draw("2,same");
    gratio->Draw("P,same");

    TLine * l = addObject(new TLine(mc.getXMin(),1,mc.getXMax(),1));
    l->Draw("same");
    float xmax=containers_[dataentry].getXMax();
    float xmin=containers_[dataentry].getXMin();
    if(gPad && containers_[dataentry].getNBins() >0){
        float yrange=fabs(gPad->GetUymax()-gPad->GetUymin());
        float xrange = fabs(xmax-xmin);
        if(gotuf){
            TLatex * la = addObject(new TLatex(containers_[dataentry].getBinCenter(1)-xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
            la->SetTextSize(la->GetTextSize() * multiplier * resizelabels);
            la->Draw("same");
        }
        if(gotof){
            TLatex * la2 = addObject(new TLatex(containers_[dataentry].getBinCenter(containers_[dataentry].getNBins())+xrange*0.06,gPad->GetUymin()-0.15*yrange,"#leq"));
            la2->SetTextSize(la2->GetTextSize() * multiplier * resizelabels);
            la2->Draw("same");
        }
    }
}
/**
 * requires all norms=1
 * returns at 0: left to right
 *         at 1: right to left
 *   idx is index of signal. data is being ignored
 */
std::vector<container1D> containerStack::getPEffPlots(size_t idx)const{
    if(idx>=containers_.size()){
        std::cout << "containerStack::getPEffPlot: idx out of range, return empty" <<std::endl;
        return std::vector<container1D>();
    }
    container1D sigcont=containers_.at(0);
    sigcont.clear();
    container1D bgcont=sigcont;
    container1D peff=sigcont; //, ...
    //sig,bg
    for(size_t i=0;i<containers_.size();i++){
        if(i==idx)
            sigcont=containers_.at(i);
        else if(legends_.at(i)!=dataleg_)
            bgcont+=containers_.at(i);
    }
    //cont ready
    //make lhs
    size_t binssize=bgcont.getBins().size();
    double integralsig=0,integralbg=0;

    double fullsignintegral=sigcont.integral(true);
    //NOT PARALLEL! NEEDS TO BE SEQUENTIAL
    for(size_t i=0;i<binssize;i++){
        integralsig+=sigcont.getBinContent(i);
        integralbg+=bgcont.getBinContent(i);
        double content=integralsig/fullsignintegral * integralsig/(integralsig+integralbg);
        if(content!=content)content=0;
        peff.getBin(i).setContent(content); //eff * purity
    }
    // HERE WE ARE  peff.
    peff.setNames("p*eff","","P*#epsilon");
    std::vector<container1D> out;
    out.push_back(peff);

    integralsig=0;integralbg=0;
    for(size_t i=binssize;i>0;i--){
        integralsig+=sigcont.getBinContent(i-1);
        integralbg+=bgcont.getBinContent(i-1);
        double content=integralsig/fullsignintegral * integralsig/(integralsig+integralbg);
        if(content!=content)content=0;
        peff.getBin(i-1).setContent(content); //eff * purity
    }
    out.push_back(peff);

    return out; //default
}

void containerStack::drawPEffPlot(TString name,double resizelabels){
    if(signals_.size()<1){
        std::cout << "containerStack::drawPEffPlot: no signal set, doing nothing" <<std::endl;
        return;
    }
    if(containers_.size()<1){
        std::cout << "containerStack::drawPEffPlot: no 1dmin containers available. Doing nothing" <<std::endl;
        return;
    }
    if(debug)
        std::cout << "containerStack::drawPEffPlot: drawing..." <<std::endl;
    std::vector<size_t> sidx=getSignalIdxs();
    TH1::AddDirectory(false);
    std::vector<std::vector<container1D> > plotss;
    double ymax=0;
    for(size_t i=0;i<sidx.size();i++){
        std::vector<container1D> plots=getPEffPlots(sidx.at(i));
        for(size_t j=0;j<plots.size();j++)
            if(ymax<plots.at(j).getYMax(false)) ymax=plots.at(j).getYMax(false);
        plotss.push_back(plots);
    }
    for(size_t i=0;i<sidx.size();i++){
        std::vector<container1D> & plots=plotss.at(i);
        if(plots.size()>1){
            if(i==0){
                float multiplier=1;
                double ylow,yhigh,xlow,xhigh;
                if(gPad){
                    gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
                    multiplier = (float)1/(yhigh-ylow);
                }
                plots.at(0).setLabelSize(resizelabels*multiplier);
            }
            TH1D * h1=addObject(plots.at(0).getTH1D("",false,true));
            TH1D * h2=addObject(plots.at(1).getTH1D("",false,true));
            h2->SetLineStyle(9);
            h1->SetLineColor(colors_.at(sidx.at(i)));
            h2->SetLineColor(colors_.at(sidx.at(i)));
            h1->SetLineWidth(2);
            h2->SetLineWidth(2);
            h1->SetMarkerStyle(1);
            h2->SetMarkerStyle(1);
            h1->SetOption("");
            h2->SetOption("");
            if(i==0){
                h1->GetYaxis()->SetNdivisions(505);
                h1->GetYaxis()->SetRangeUser(ymax*0.00000000001,ymax*1.05);
                h1->Draw();
            }
            else{
                h1->Draw("same");
            }
            h2->Draw("same");
        }
    }

}


void containerStack::drawSBGPlot(TString name,double resizelabels,bool invert){
    if(!checkDrawDimension()){
        std::cout << "containerStack::drawRatioPlot: only available for 1dim stacks!" <<std::endl;
        return;
    }
    if(debug)
        std::cout << "containerStack::drawSBGPlot: drawing..." <<std::endl;
    TH1::AddDirectory(false);
    //prepare container
    if(name=="") name=name_;
    //int dataentry=0;
    bool gotdentry=false;
    bool gotuf=false;
    bool gotof=false;
    for(unsigned int i=0;i<size();i++){ // get datalegend and check if underflow or overflow in any container
        if(getLegend(i) == dataleg_){
            //dataentry=i;
            gotdentry=true;
            if(gotof && gotuf) break;
        }
        if(containers_[i].getOverflow() < -0.9){
            gotof=true;
            if(gotdentry && gotuf) break;
        }
        if(containers_[i].getUnderflow() < -0.9){
            gotuf=true;
            if(gotdentry && gotof) break;
        }
    }
    //prepare container
    std::vector<container1D> vsgbg;
    double totmax=-999;
    for(size_t i=0;i<legends_.size();i++){
        if(legends_[i]==dataleg_) continue;
        container1D sig=containers_[i];
        container1D bg=sig;
        bg.clear();
        container1D sgbg=bg;
        //get all other contributions
        for(size_t j=0;j<legends_.size();j++){
            if(i==j) continue;
            if(legends_[j]==dataleg_) continue;
            bg += containers_[j];
        }
        sig.setAllErrorsZero();
        std::vector<float> bins=sig.getBins();
        double sinteg=0, bginteg=0;
        for(size_t j=0;j<bins.size();j++){
            size_t bin=j;
            if(invert) bin=bins.size()-1-j;
            sinteg  += sig.getBinContent(bin);
            bginteg += bg.getBinContent(bin);
            double snr=sinteg/(sinteg+bginteg);
            if(snr!=snr) snr=0; //nan protection
            if(totmax < snr) totmax=snr;
            sgbg.setBinContent(bin,snr);
        }
        sgbg.normalize();
        vsgbg.push_back(sgbg);
    }

    float multiplier=1;
    double ylow,yhigh,xlow,xhigh;
    if(gPad){
        gPad->GetPadPar(xlow,ylow,xhigh,yhigh);
        multiplier = (float)1/(yhigh-ylow);
    }
    bool firstdraw=true;
    for(size_t i=0;i<vsgbg.size();i++){
        //get TH1Ds
        container1D * c=&vsgbg[i];
        if(firstdraw)
            c->setLabelSize(multiplier * resizelabels);
        TH1D * h=addObject(c->getTH1D(""));
        h->GetYaxis()->SetRangeUser(0,totmax*1.05);
        h->SetLineColor(colors_[i]);
        h->SetLineWidth(2);
        if(firstdraw){
            if(invert)
                h->GetYaxis()->SetTitle("#frac{sg}{sg+bg}#leftarrow");
            else
                h->GetYaxis()->SetTitle("#frac{sg}{sg+bg}#rightarrow");
            h->Draw();
            firstdraw=false;
        }
        else{
            h->Draw("same");
        }
    }
}
/*
TCanvas * containerStack::makeTCanvas(bool drawratioplot){
	if(drawratioplot) return makeTCanvas(ratio);
	else return makeTCanvas(normal);
}
 */
TCanvas * containerStack::makeTCanvas(plotmode plotMode){
    if(!checkDrawDimension()){
        std::cout << "containerStack::makeTCanvas: only available for 1dim stacks!" <<std::endl;
        return 0;
    }
    TH1::AddDirectory(false);
    TString canvasname=name_+"_c";
    if(mode==unfolddim1)
        canvasname = canvasname + "_uf";
    TCanvas * c = new TCanvas(canvasname,canvasname);
    if(containerStack::batchmode)
        c->SetBatch();
    TH1D * htemp=(new TH1D("sometemphisto"+name_,"sometemphisto"+name_,2,0,1)); //creates all gPad stuff etc and prevents seg vio, which happens in some cases; weird
    htemp->Draw();
    if(debug)
        std::cout << "containerStack::makeTCanvas: prepared canvas "<< canvasname << ", drawing plots" <<std::endl;
    gStyle->SetOptTitle(0);
    if(plotMode == normal){
        c->cd();
        drawControlPlot();
    }
    else{
        double labelresize=1.;
        double divideat=0.3;
        c->Divide(1,2);
        c->SetFrameBorderSize(0);
        c->cd(1)->SetLeftMargin(0.15);
        c->cd(1)->SetBottomMargin(0.03);
        c->cd(1)->SetPad(0,divideat,1,1);
        drawControlPlot(name_,false,labelresize);
        c->cd(2)->SetBottomMargin(0.4 * 0.3/divideat);
        c->cd(2)->SetLeftMargin(0.15);
        c->cd(2)->SetTopMargin(0);
        c->cd(2)->SetPad(0,0,1,divideat);
        if(plotMode == ratio){
            drawRatioPlot("",labelresize);
            c->cd(2)->RedrawAxis();
        }
        else if(plotMode == sigbg)
            drawPEffPlot("",labelresize);
        c->cd(1)->RedrawAxis();
    }
    /*else{
		c->Divide(1,3);
		c->SetFrameBorderSize(0);
		c->cd(1)->SetLeftMargin(0.15);
		c->cd(1)->SetBottomMargin(0.0);
		c->cd(2)->SetLeftMargin(0.15);
		c->cd(2)->SetBottomMargin(0.0);
		c->cd(3)->SetLeftMargin(0.15);
		c->cd(3)->SetBottomMargin(0.4);
		drawControlPlot(name_,false,1);
		c->cd(2);
		drawSBGPlot("",1,false);
		c->cd(3);
		drawSBGPlot("",1,true);
		c->cd(1)->RedrawAxis();
	}*/
    if(gStyle){
        gStyle->SetOptTitle(0);
        gStyle->SetOptStat(0);
    }
    delete htemp;
    return c;
}

std::map<TString, float> containerStack::getAllContentsInBin(size_t bin,bool print)const{
    if(containers_.size() < 1 || bin >= containers_.at(0).getBins().size()){
        throw std::out_of_range("containerStack::coutAllInBin: bin out of range (getNBins()) or no 1D content");
    }
    std::map<TString, float>  out;
    if(print) std::cout << "Content of bin " << bin << " for containerStack " << name_ << std::endl;
    for(size_t i=0;i<legends_.size();i++){
        if(print) std::cout <<legends_.at(i) << ": " << containers_.at(i).getBinContent(bin) << std::endl;
        out[legends_.at(i)] = containers_.at(i).getBinContent(bin);
    }
    return out;
}


containerStack containerStack::rebinXToBinning(const std::vector<float> & newbins)const{

    containerStack out=*this;
    for(size_t i=0;i<containers_.size();i++)
        out.containers_.at(i)=containers_.at(i).rebinToBinning(newbins);

    for(size_t i=0;i<containers2D_.size();i++)
        out.containers2D_.at(i)=containers2D_.at(i).rebinXToBinning(newbins);

    for(size_t i=0;i<containers1DUnfold_.size();i++)
        out.containers1DUnfold_.at(i)=containers1DUnfold_.at(i).rebinToBinning(newbins);

    return out;
}


containerStack containerStack::rebinYToBinning(const std::vector<float> & newbins)const{
    containerStack out=*this;
    for(size_t i=0;i<containers2D_.size();i++)
        out.containers2D_.at(i)=containers2D_.at(i).rebinYToBinning(newbins);
    return out;
}


//just perform functions on the containers with same names
ztop::containerStack containerStack::operator + (containerStack stack){
    if(mode!=stack.mode){
        std::cout << "containerStack::operator +: stacks must be of same dimension, returning input" << std::endl;
        return stack;
    }
    for(unsigned int i=0;i<containers_.size();i++){
        for(unsigned int j=0;j<stack.containers_.size();j++){
            if(containers_[i].getName() == stack.containers_[j].getName()){
                stack.containers_[j] += containers_[i];
            }
        }
    }
    for(unsigned int i=0;i<containers2D_.size();i++){
        for(unsigned int j=0;j<stack.containers2D_.size();j++){
            if(containers2D_[i].getName() == stack.containers2D_[j].getName()){
                stack.containers2D_[j] = containers2D_[i] + stack.containers2D_[j];
            }
        }
    }
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        for(unsigned int j=0;j<stack.containers1DUnfold_.size();j++){
            if(containers1DUnfold_[i].getName() == stack.containers1DUnfold_[j].getName()){
                stack.containers1DUnfold_[j] = containers1DUnfold_[i] + stack.containers1DUnfold_[j];
            }
        }
    }
    return stack;
}
ztop::containerStack containerStack::operator - (containerStack stack){
    if(mode!=stack.mode){
        std::cout << "containerStack::operator -: stacks must be of same dimension, returning input" << std::endl;
        return stack;
    }
    for(unsigned int i=0;i<containers_.size();i++){
        for(unsigned int j=0;j<stack.containers_.size();j++){
            if(containers_[i].getName() == stack.containers_[j].getName()){
                stack.containers_[j] = containers_[i] - stack.containers_[j];
            }
        }
    }
    for(unsigned int i=0;i<containers2D_.size();i++){
        for(unsigned int j=0;j<stack.containers2D_.size();j++){
            if(containers2D_[i].getName() == stack.containers2D_[j].getName()){
                stack.containers2D_[j] = containers2D_[i] - stack.containers2D_[j];
            }
        }
    }
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        for(unsigned int j=0;j<stack.containers1DUnfold_.size();j++){
            if(containers1DUnfold_[i].getName() == stack.containers1DUnfold_[j].getName()){
                stack.containers1DUnfold_[j] = containers1DUnfold_[i] - stack.containers1DUnfold_[j];
            }
        }
    }
    return stack;
}
ztop::containerStack containerStack::operator / (containerStack  stack){
    if(mode!=stack.mode){
        std::cout << "containerStack::operator /: stacks must be of same dimension, returning input" << std::endl;
        return stack;
    }
    for(unsigned int i=0;i<containers_.size();i++){
        for(unsigned int j=0;j<stack.containers_.size();j++){
            if(containers_[i].getName() == stack.containers_[j].getName()){
                stack.containers_[j] = containers_[i] / stack.containers_[j];
            }
        }
    }
    for(unsigned int i=0;i<containers2D_.size();i++){
        for(unsigned int j=0;j<stack.containers2D_.size();j++){
            if(containers2D_[i].getName() == stack.containers2D_[j].getName()){
                stack.containers2D_[j] = containers2D_[i] / stack.containers2D_[j];
            }
        }
    }
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        for(unsigned int j=0;j<stack.containers1DUnfold_.size();j++){
            if(containers1DUnfold_[i].getName() == stack.containers1DUnfold_[j].getName()){
                stack.containers1DUnfold_[j] = containers1DUnfold_[i] / stack.containers1DUnfold_[j];
            }
        }
    }
    return stack;
}
ztop::containerStack containerStack::operator * (containerStack  stack){
    if(mode!=stack.mode){
        std::cout << "containerStack::operator /: stacks must be of same dimension, returning input" << std::endl;
        return stack;
    }
    for(unsigned int i=0;i<containers_.size();i++){
        for(unsigned int j=0;j<stack.containers_.size();j++){
            if(containers_[i].getName() == stack.containers_[j].getName()){
                stack.containers_[j] = containers_[i] * stack.containers_[j];
            }
        }
    }
    for(unsigned int i=0;i<containers2D_.size();i++){
        for(unsigned int j=0;j<stack.containers2D_.size();j++){
            if(containers2D_[i].getName() == stack.containers2D_[j].getName()){
                stack.containers2D_[j] = containers2D_[i] * stack.containers2D_[j];
            }
        }
    }
    for(unsigned int i=0;i<containers1DUnfold_.size();i++){
        for(unsigned int j=0;j<stack.containers1DUnfold_.size();j++){
            if(containers1DUnfold_[i].getName() == stack.containers1DUnfold_[j].getName()){
                stack.containers1DUnfold_[j] = containers1DUnfold_[i] * stack.containers1DUnfold_[j];
            }
        }
    }
    return stack;
}
ztop::containerStack containerStack::operator * (double scalar){
    ztop::containerStack out=*this;
    for(unsigned int i=0;i<containers_.size();i++) out.containers_[i] = containers_[i] * scalar;
    for(unsigned int i=0;i<containers2D_.size();i++) out.containers2D_[i] = containers2D_[i] * scalar;
    for(unsigned int i=0;i<containers1DUnfold_.size();i++) out.containers1DUnfold_[i] = containers1DUnfold_[i] * scalar;
    return out;
}
ztop::containerStack containerStack::operator * (float scalar){
    return *this * (double)scalar;
}
ztop::containerStack containerStack::operator * (int scalar){
    return *this * (double)scalar;
}


bool containerStack::setsignal(const TString& sign){
    for(size_t i=0;i<legends_.size();i++){
        if(sign == legends_[i]){
            signals_.clear();
            signals_.push_back(sign);
            return true;
        }
    }
    std::cout << "containerStack::setsignal: signalname " << sign << " not found in legend entries. doing nothing" <<std::endl;
    return false;
}
bool containerStack::addSignal(const TString& sign){
    if(std::find(signals_.begin(),signals_.end(),sign)!=signals_.end())
        return true; //already in
    for(size_t i=0;i<legends_.size();i++){
        if(sign == legends_[i]){
            signals_.push_back(sign);
            return true;
        }
    }
    std::cout << "containerStack::setsignal: signalname " << sign << " not found in legend entries. doing nothing" <<std::endl;
    return false;
}
bool containerStack::setsignals(const std::vector<TString> & signs){
    if(!checkSignals(signs)){
        std::cout << "containerStack::setsignals: not all signals found in legend entries" <<std::endl;
        return false;
    }
    signals_=signs;
    return true;
}
std::vector<size_t> containerStack::getSignalIdxs() const{
    std::vector<size_t> out;
    for(size_t i=0;i<signals_.size();i++){
        out.push_back((size_t)(std::find(legends_.begin(),legends_.end(),signals_.at(i)) - legends_.begin()));
    }
    return out;
}

container1D containerStack::getSignalContainer()const{
    if(mode != dim1 && mode !=unfolddim1){
        if(debug) std::cout << "containerStack::getSignalContainer: return dummy" <<std::endl;
        return container1D();
    }
    std::vector<size_t> sidx=getSignalIdxs();
    container1D out;
    if(sidx.size()>0) out=getContainer(sidx.at(0));
    for(size_t i=1;i<sidx.size();i++)
        out+=getContainer(sidx.at(i));
    return out;
}
container1D containerStack::getBackgroundContainer()const{
    if(mode != dim1 && mode !=unfolddim1){
        if(debug) std::cout << "containerStack::getBackgroundContainer: return dummy" <<std::endl;
        return container1D();
    }
    std::vector<size_t> sidx=getSignalIdxs();
    container1D out;
    bool first=true;
    for(size_t i=0;i<containers_.size();i++){
        if(std::find(sidx.begin(),sidx.end(),i) != sidx.end()){
            continue;
        }
        if(first){
            out=containers_.at(i);
            first=false;
        }
        else{
            out+=containers_.at(i);
        }
    }

    return out;

}


container1DUnfold containerStack::produceUnfoldingContainer(const std::vector<TString> &sign) const{// if sign empty use "setsignal"
    /*
   ad all signal -> cuf
   add all recocont_ w/o signaldef -> set BG of cuf
   (add all) with datadef -> setData in cuf
     */


    container1DUnfold out;
    if(mode != unfolddim1 || containers1DUnfold_.size()<1){
        std::cout << "containerStack::produceUnfoldingContainer: No unfolding information, return empty" <<std::endl;
        return out;
    }
    if(debug)
        checknorms();
    bool temp=container1DUnfold::c_makelist;
    container1DUnfold::c_makelist=false;
    bool temp2=container1D::c_makelist;
    container1D::c_makelist=false;
    bool temp3=histoContent::addStatCorrelated;
    histoContent::addStatCorrelated=false;

    std::vector<TString> signals=signals_;

    if(sign.size()>0)
        signals=sign;
    out=containers1DUnfold_.at(0);
    out.clear();
    container1D background=containers1DUnfold_.at(0).getRecoContainer();
    background.clear();
    container1D data=background;

    //find signal

    for(size_t leg=0;leg<legends_.size();leg++){
        bool issignal=false;
        const container1DUnfold * cuf=&containers1DUnfold_.at(leg);
        for(size_t sig=0;sig<signals.size();sig++){
            if(signals.at(sig) == legends_.at(leg)){
                if(debug)
                    std::cout << "containerStack::produceUnfoldingContainer: identified " << legends_.at(leg) << " as signal -> add to signal, norm: "<< norms_[leg] <<std::endl;
                container1DUnfold normcuf= *cuf;
                normcuf = normcuf*norms_[leg];
                out = out + normcuf;
                issignal=true;
                break;
            }
        }
        if(!issignal && legends_.at(leg) != dataleg_){
            if(debug)
                std::cout << "containerStack::produceUnfoldingContainer: identified as BG: "<< legends_.at(leg) <<  ", norm: "<< norms_[leg] << std::endl;
            container1D bg=cuf->getRecoContainer();
            background = background + bg*norms_[leg];
        }
        if(legends_.at(leg) == dataleg_){
            if(debug)
                std::cout << "containerStack::produceUnfoldingContainer: identified as data: "<< legends_.at(leg) << std::endl;
            container1D d=cuf->getRecoContainer();
            data = data + d*norms_[leg];
        }
    }
    //migration matrix, eff, background prepared -> finished
    //background needs to be ADDED!!!
    out.setBackground(out.getBackground()+background);
    out.setRecoContainer(data);
    container1DUnfold::c_makelist=temp;
    container1D::c_makelist=temp2;
    histoContent::addStatCorrelated=temp3;

    return out;
}
container1DUnfold containerStack::produceXCheckUnfoldingContainer(const std::vector<TString> & sign) const{ /* if sign empty use "setsignal"
   just adds all with signaldef (to be then unfolded and a comparison plot to be made gen vs unfolded gen) -> SWITCH OFF SYSTEMATICS HERE! (should be independent)

 */
    //define operators (propagate from containers) for easy handling
    // also define += etc. and use container += if better, needs to be implemented (later)
    container1DUnfold out;
    if(mode != unfolddim1 || containers1DUnfold_.size()<1){
        std::cout << "containerStack::produceXCheckUnfoldingContainer: No unfolding information, return empty" <<std::endl;
        return out;
    }
    if(debug)
        checknorms();

    bool temp=container1DUnfold::c_makelist;
    container1DUnfold::c_makelist=false;
    bool temp2=container1D::c_makelist;
    container1D::c_makelist=false;
    bool temp3=histoContent::addStatCorrelated;
    histoContent::addStatCorrelated=false;

    std::vector<TString> signals=signals_;
    if(sign.size()>0)
        signals=sign;
    out=containers1DUnfold_.at(0);
    out.clear();
    for(size_t leg=0;leg<legends_.size();leg++){
        const container1DUnfold * cuf=&containers1DUnfold_.at(leg);
        for(size_t sig=0;sig<signals.size();sig++){
            if(signals.at(sig) == legends_.at(leg)){ //so damn not fast
                container1DUnfold normcuf=*cuf;
                normcuf = normcuf*norms_[leg];
                out = out + normcuf;
                break;
            }
        }
    }
    container1DUnfold::c_makelist=temp;
    container1D::c_makelist=temp2;
    histoContent::addStatCorrelated=temp3;
    return out;
}

bool containerStack::checknorms() const{
    bool out=true;
    for(size_t i=0;i<norms_.size();i++){
        if(norms_[i] != 1){
            if(debug){
                std::cout << "containerStack::checknorms: Norm for " << legends_[i] << " is not 1; " << norms_[i] << std::endl;
                out=false;
            }
            else{
                return false;
            }
        }
    }
    return out;
}


container1DUnfold containerStack::getSignalContainer1DUnfold(const std::vector<TString> &sign) const{
    bool mklist=container1DUnfold::c_makelist;
    container1DUnfold::c_makelist=false;
    if(mode!=unfolddim1){
        std::cout << "containerStack::getSignalContainer1DUnfold: bo container1DUnfold content, return empty" <<std::endl;
        container1DUnfold c;
        container1DUnfold::c_makelist=mklist;
        return c;
    }
    std::vector<TString> signals=sign;
    if(signals.size()<1)
        signals=signals_;
    container1DUnfold out;
    if(containers1DUnfold_.size()>0)
        out=containers1DUnfold_.at(0);
    out.clear();
    for(size_t i=0;i<signals.size();i++){
        container1DUnfold ctemp=getContribution1DUnfold(signals.at(i));
        out = out + ctemp;
    }

    container1DUnfold::c_makelist=mklist;
    return out;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////private member functions////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * rewrite: always true
 */
int containerStack::checkLegOrder() const{ //depre
    return 0;
    /*
	for(std::map<TString,size_t>::const_iterator it=legorder_.begin();it!=legorder_.end();++it){
		if(it->second > size()){
			std::cout << "containerStack::checkLegOrder: "<< name_ << ": legend ordering numbers (" <<it->second << ") need to be <= size of stack! ("<< size()<<")" <<std::endl;
			return -1;
		}
	}
	return 0; */
}
/**
 * might need a rewrite
 */
std::vector<size_t> containerStack::sortEntries(/* std::vector<int> inputlegord OR map with legnames?, */bool inverse) const{

    //new implementation
    std::vector<int> inputlegord=legorder_;
    std::vector<int> sortedilo=inputlegord;
    if(inverse)
        std::sort(sortedilo.begin(),sortedilo.end(),std::greater<int>());
    else
        std::sort(sortedilo.begin(),sortedilo.end());

    std::vector<size_t> legasso;
    //protect size etc

    //BULLSHIT
    for(size_t i=0;i<sortedilo.size();i++){
        int oval=sortedilo.at(i);
        for(size_t j=0;j<inputlegord.size();j++){//search in internal ordering
            if(oval==inputlegord.at(j)){ //has position j
                legasso.push_back(j);
                break;
            }
        }
    }
    //fill not asso
    for(size_t i=legasso.size();i<legends_.size();i++)
        legasso.push_back(i);

    if(debug){
        std::cout << "containerStack::sortEntries: ";
        for(size_t i=0;i<legasso.size();i++)
            std::cout << legasso.at(i) << " " << legends_.at(i) << " ";
        std::cout<<std::endl;
    }

    return legasso;
    ///end new implementation

    /*
	std::vector<int> ordering;
	std::vector<size_t> dontuse;

	if(legorder_.size()<1){ //not filled
		std::vector<size_t> easyorder;
		for(size_t i=0;i<legends_.size();i++)
			easyorder.push_back(i);
		return easyorder;
	}

	ordering.resize(size(),-1);
	for(unsigned int it=0;it<size();it++){
		if(checkLegOrder()>-1 && legorder_.find(legends_[it])!=legorder_.end()){
			ordering.at(legorder_.find(legends_[it])->second)=it;
			dontuse.push_back(it);
		}
	}


	for(size_t it=0;it<ordering.size();it++){ //fill rest
		int ord=ordering.at(it);
		if(ord<0){ //not yet filled
			for(size_t newit=0;newit<size();newit++){
				bool use=true;
				for(size_t j=0;j<dontuse.size();j++){
					if(newit == dontuse.at(j))
						use=false;
				}
				if(use){
					ordering.at(it)=(int)newit;
				}
			}
		}
	}

	std::vector<size_t> out;

	if(inverse){
		out.resize(ordering.size(),0);
		size_t outidx=ordering.size()-1;
		for(size_t i=0;i<ordering.size();i++){
			out.at(outidx)=ordering.at(i);
			outidx--;
		}
	}
	else{
		for(size_t i=0;i<ordering.size();i++)
			out.push_back((size_t) ordering.at(i));
	}
	return out;
     */
}

bool containerStack::checkDrawDimension() const{
    return (mode==dim1 || mode==unfolddim1);
}

bool containerStack::checkSignals(const std::vector<TString> &signs) const{
    size_t count=0;
    for(size_t j=0;j< signs.size();j++){
        bool succ=false;
        const TString & sign=signs.at(j);
        for(size_t i=0;i<legends_.size();i++){
            if(sign == legends_[i]){
                succ=true;
                break;
            }
        }
        if(succ)
            count++;
    }
    if(count != signs.size()){
        return false;
    }
    return true;
}

}//namespace
