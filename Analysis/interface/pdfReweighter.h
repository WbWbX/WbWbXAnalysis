/*
 * pdfReweighter.h
 *
 *  Created on: May 28, 2014
 *      Author: kiesej
 */

#ifndef PDFREWEIGHTER_H_
#define PDFREWEIGHTER_H_

#include "../interface/simpleReweighter.h"
#include <cstddef>

namespace ztop{

class NTEvent;
/**
 * different modes:
 * - total variation (also reweights the 0 eigenvector)
 * - only difference wrt to 0 eigenvector:
 *   (weight = ev(X,event)/ev(0, sameevent))
 *
 * - for both call getRenormalization() at the end to only evaluate
 *   shape differences in initial generated events
 *
 *   needs: to be renormalized to all processed events, not only the ones
 *   in event loop... implement and make safe wrt base class
 */
class pdfReweighter: public simpleReweighter{
public:
	pdfReweighter();
	~pdfReweighter();

	void setPdfIndex(size_t idx){
		pdfidx_=idx;
		switchOff(false);
	}
	void setNTEvent(NTEvent * evt){ntevent_=evt;}

	void reWeight( float &oldweight);

	void setReweightToNominalEigenvector(bool set){rewtonomev_=set;}

	bool getReweightToNominalEigenvector(){return rewtonomev_;}

	const size_t& getConfIndex(){return pdfidx_;}

private:

	void setNewWeight(const float &w){simpleReweighter::setNewWeight(w);}

	NTEvent * ntevent_;
	size_t pdfidx_;
	bool rewtonomev_;
};


}



#endif /* PDFREWEIGHTER_H_ */
