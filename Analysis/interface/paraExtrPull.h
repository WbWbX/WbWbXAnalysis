/*
 * paraExtrPull.h
 *
 *  Created on: Aug 15, 2014
 *      Author: kiesej
 */

#ifndef PARAEXTRPULL_H_
#define PARAEXTRPULL_H_
#include "WbWbXAnalysis/Tools/interface/parameterExtractor.h"
#include "WbWbXAnalysis/Tools/interface/histo1D.h"
#include "WbWbXAnalysis/Tools/interface/graph.h"
#include <vector>
#include "TRandom3.h"
#include "WbWbXAnalysis/Tools/interface/graphFitter.h"

namespace ztop{

class paraExtrPull {
public:
	paraExtrPull():ex_(0),evalpoint_(0),datascale_(1.),corrmcscale_(1.),niter_(1000),cont_(0),random_(0),failcount_(0){
		points_.push_back(166.5);
		points_.push_back(169.5);
		points_.push_back(171.5);
		points_.push_back(172.5);
		points_.push_back(173.5);
		points_.push_back(175.5);
		points_.push_back(178.5);
	};

	void setExtractor(parameterExtractor *ex){ex_=ex;}


	/**
	 * function will be f(x)= offset+ ax + bx^2
	 */

	void setEvalPoint(float nompoint){evalpoint_=nompoint;}
	void setGeneratePoints(std::vector<float> xvals){points_=xvals;}

	void setCorrelatedMCInput(const std::vector<graph> & g){corrtrueinput_=g;}
	void setUncorrelatedMCInput(const std::vector<graph> & g){uncorrtrueinput_=g;}
	void setDataGraphInput(const std::vector<graph> & g){datatrueinput_=g;}
	void setFitParasTrue(const std::vector<std::vector<double> > & paras){externalfitparas_=paras;}

	/**
	 * statistics! used to get right "poisson scaling"
	 * default 1
	 */
	void setDataScale(const float &ds){datascale_.clear();datascale_.push_back(ds);}
	/**
	 * statistics! used to get right "poisson scaling"
	 * default 1
	 */
	void setMCScale(const float &ds){corrmcscale_.clear();corrmcscale_.push_back(ds);}

	float getFunctionout(float xval,size_t idx=0)const;

	void setNIterations(size_t nit){niter_=nit;}

	void setFillContainer1D(histo1D * c){cont_=c;}

	void setRandom(TRandom3* r){random_=r;}

	void fill();

private:

	graph generateMCPoints(size_t idx=0)const;
	graph generateDataPoints(size_t idx=0)const;

	parameterExtractor* ex_;

	std::vector<graphFitter> fits_;
	std::vector<std::vector<double> > externalfitparas_;
	float evalpoint_;

	std::vector<float> points_;

	std::vector<float>  datascale_,corrmcscale_,uncorrmcscale_;
	size_t niter_;

	histo1D* cont_;
	TRandom3* random_;

	std::vector<graph> uncorrtrueinput_,corrtrueinput_,datatrueinput_;
	/**
	 * real corr = getFunction * reluncorrcontr_;
	 * real uncorr = getFunction * (1-reluncorrcontr_);
	 */
	std::vector<float> reluncorrcontr_;
	size_t failcount_;
};



}



#endif /* PARAEXTRPULL_H_ */
