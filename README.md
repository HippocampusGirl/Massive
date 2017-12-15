# Massive
Tools for the analysis of large neuroimaging datasets.

### m_brainconnectivity ###
Calculate network properties of large-scale brain connectivity data.
Build using ```make m_brainconnectivity```, after adding FSL (http://fsl.fmrib.ox.ac.uk) to ```LIBRARY_PATH``` and ```C_INCLUDE_PATH```. Requires gcc and OpenBLAS, or alternatively icc.

```
Usage: m_brainconnectivity -i/-p <input> -o <output> <options>

Required arguments:
-i <filename> input 4d image
-p <filename> alternatively, input text file where rows are time,
columns are nodes
-o <prefix> output prefix

Options (one or more of each):
-n <network_definition_scheme> specify how networks are constructed.
All methods lead to weighted undirected networks, weights are scaled
as correlations. The following network definition schemes are available:
   corr pearson correlation
   ridge ridge-regularized partial correlation with rho=1.0
   ridge:<rho> ridge-regularized partial correlation with user-specified rho

-t <threshold_type>:<value> specify how the network is thresholded. The
following threshold types are available:
   absolute all weights above the value are retained
   proportional a proportion of the strongest weights is retained
   nnegproportional also removes negative weights, important for pathlength
The value can be specified either directly as a number, or as a range
as <lthr>:<step>:<uthr>. For example, absolute:-1.0 is equivalent to no
thresholding.

-m <measure> specify which network properties are calculated. The following
measures are available:
   global:clustering_coef
   global:charpath
   global:efficiency

-d enable debug messages, however these are not very useful at present
```

m_brainconnectivity supports parallel processing via OpenMP. To enable that please run ```export OMP_NUM_THREADS=<how many cores to use>``` before using the program.

If you are working on a computer with multiple CPUs, you can further increase performance by making the program aware of how the cores are distributed across the different CPUs.
```
export OMP_NESTED=TRUE
export OMP_PLACES=cores
export OMP_PROC_BIND=spread,close
SOCKETS=$(numactl -H | grep nodes | awk '{printf $2}')
CORES_PER_SOCKET=$(numactl -H | grep "node 0 cpus" | awk '(NF>3) {printf (NF-3)/2}')
export OMP_NUM_THREADS=${SOCKETS},${CORES_PER_SOCKET}
export OMP_PLACES=$(numactl -H | grep cpus | awk '(NF>3) {for (i = 4; i <= NF; i++) printf "%d,", $i}' | sed 's/.$//')
export OMP_STACKSIZE=128M
# If compiled with ICC
export KMP_HOT_TEAMS=1
export KMP_HOT_TEAMS_MAX_LEVELS=2
```
