#!bin/sh -u

export CURRENT_DIR=$PWD
export FCCSW_DIR="../FCCSW"
export FCCEDM_DIR="../fcc-edm"
export PODIO_DIR="../podio"

echo "FCCSW init"
cd $FCCSW_DIR
source init.sh
echo "fcc-edm init"
cd $FCCEDM_DIR
source init.sh
echo "podio init"
cd $PODIO_DIR
source init.sh
echo "FCC_analysis_cpp setup"
cd $CURRENT_DIR
source init.sh

