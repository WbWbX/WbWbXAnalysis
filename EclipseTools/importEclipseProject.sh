ECLIPSE_PATH=`which eclipse`

if [ ! $ECLIPSE_PATH ]
then 
echo "which eclipse did not return anything - exit"
exit 1
fi

TEMP_FILE=/tmp/asdf1234edd.txt
 
DIR=`pwd`

get_script_path (){
    cd `dirname $1` > /dev/null
    DIR=`pwd`
    cd - > /dev/null
    echo $DIR
}

#
# BUG: 2009.07.22: Eclipse cannot be running when we import a project.
# Otherwise the import gets lost.
#
if ps ax | grep eclipse | grep -v grep | grep -v $0 > /dev/null; then
    echo "Eclipse is running! Please exit Eclipse!"
    exit 1
fi
 
THIS_DIR=`get_script_path $0`
 
while getopts "s:w:p:" flag
do
    case $flag in
#        s)
#            SRC_DIR=$OPTARG
#            ;;
#        w)
#            WS=$OPTARG
#            ;;
        p)
            PROJ_NAME=$OPTARG
            ;;
        *)
            echo "Unrecognized argument '$flag'!"
            exit 1
            ;;
    esac
done
 
 
SRC_DIR=$DIR/src
WS=$DIR/eclipse


[ -z $SRC_DIR ] && SRC_DIR=./
# Get absolute path of SRC_DIR
cd $SRC_DIR
SRC_DIR=`pwd`
cd - > /dev/null


if [ ! -d "$SRC_DIR" ]; then
    echo "$SRC_DIR is not a directory or it does not exist!"
    exit 1
fi

 function basedir {
    echo `pwd | awk -F\/ '{print $(NF-1)}' | sed 's/ /\//'`
}

 
if [ -z $PROJ_NAME ]; then
    echo "using project name: $"
    PROJ_NAME="$(basedir)"
fi
 

echo src = $SRC_DIR
echo ws = $WS
echo proj_name = $PROJ_NAME
 
cat $SRC_DIR/../.project_temp | sed -e "s/template-name/$PROJ_NAME/g" > $SRC_DIR/.project
cat $SRC_DIR/../.cproject_temp | sed -e "s/template-name/$PROJ_NAME/g" > $SRC_DIR/.cproject
 

$ECLIPSE_PATH -nosplash -data $WS -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import "$SRC_DIR" 2>&1 | tee $TEMP_FILE
 
OUTPUT=`cat $TEMP_FILE`
echo $OUTPUT
rm -f $TEMP_FILE

rm -f runEclipse.sh
echo "eclipse -data $WS" > runEclipse.sh

chmod +x runEclipse.sh
echo "run eclipse here with runEclipse.sh"
