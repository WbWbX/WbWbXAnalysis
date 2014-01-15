/*
 * globalPlotStyles.h
 *
 *  Created on: Oct 4, 2013
 *      Author: kiesej
 */

#ifndef GLOBALPLOTSTYLES_H_
#define GLOBALPLOTSTYLES_H_

#include "containerStyle.h"
#include "plotStyle.h"
/**
 * File to define global plot styles as templates. all are stored in a certain namespace
 */

namespace ztop{
namespace plotting{
namespace axisStyles{
//containerAxisStyle( titlesize, labelsize, titleoffset,  labeloffset,  ticklength, ndivisions, Max,  Min)

axisStyle xaxis_default(0.06,0.05,1,0.005,0.03,510,-1,1);
axisStyle yaxis_default(0.06,0.05,1,0.005,0.03,510,-1,1);


}
namespace containerStyles{
//containerStyle( markersize,  markerstyle, markercolor,  linesize,  linestyle,  linecolor, fillstyle,  fillcolor,  errorstyle)

containerStyle standard_data=containerStyle(1.,20,0,1,1,0,1001,0,containerStyle::errorBarStyles::normalErr);
containerStyle standard_mc_marker=containerStyle(1.,23,632,1,1,0,1001,0,containerStyle::errorBarStyles::normalErr);
containerStyle standard_mc_fill=  containerStyle(1.,23,632,1,1,0,3001,0,containerStyle::errorBarStyles::fillErr);


namespace arrays{
containerStyle standard_dataMCmarker[]={standard_data,standard_mc_marker};
containerStyle standard_dataMCFill[]={standard_data,standard_mc_fill};

}
}//containerStyles
namespace plotStyles{
//plotStyle( (*) cstyles, cstylesSize,  xaxis,  yaxis, bottommargin, topmargin, leftmargin, rightmargin, dividebybinwidth, horizontalplot=false)

plotStyle singlePlot_data=plotStyle(&containerStyles::standard_data,1,axisStyles::xaxis_default,axisStyles::yaxis_default,0.15,0.1,0.15,0.1,true,false);
plotStyle singlePlot_mc_marker=plotStyle(&containerStyles::standard_mc_marker,1,axisStyles::xaxis_default,axisStyles::yaxis_default,0.15,0.1,0.15,0.1,true,false);
plotStyle singlePlot_mc_fill=plotStyle(&containerStyles::standard_mc_fill,1,axisStyles::xaxis_default,axisStyles::yaxis_default,0.15,0.1,0.15,0.1,true,false);

plotStyle data_mcmarker=plotStyle(&containerStyles::arrays::standard_dataMCmarker,2,axisStyles::xaxis_default,axisStyles::yaxis_default,0.15,0.1,0.15,0.1,true,false);
plotStyle data_mcfill=plotStyle(&containerStyles::arrays::standard_dataMCFill,2,axisStyles::xaxis_default,axisStyles::yaxis_default,0.15,0.1,0.15,0.1,true,false);


namespace arrays{

}
}//plotStyles
/*
namespace canvasStyles{

canvasStyle simple_datamc=canvasStyle();


}
*/
}//plotting
}//ztop

#endif /* GLOBALPLOTSTYLES_H_ */
