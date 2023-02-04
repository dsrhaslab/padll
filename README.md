## PADLL: Programmable and Adaptable I/O for Dynamically Loaded Libraries

<h1 align="left">
  <!-- logo as of current commit -->
  <img src="https://img.shields.io/badge/C++-17-yellow.svg?style=flat&logo=c%2B%2B" />
  <img src="https://img.shields.io/badge/status-research%20prototype-green.svg" />
  <a href="https://opensource.org/licenses/BSD-3-Clause">
    <img src="https://img.shields.io/badge/license-BSD--3-blue.svg" />
  </a>
</h1>

PADLL is a storage middleware that enables system administrators to proactively and holistically control the rate of data and and metadata workflows to achieve QoS in HPC storage systems. 
Its design if build under the following core principles:

* <b>Application and File System agnostic:</b> PADLL does not require code changes to any core layer of the HPC I/O stack, being agnostic of the applications it is controlling as well as the file system to which requests are submitted to, making it generally applicable and POSIX-compliant. PADLL achieves this using LD_PRELOAD.
* <b>Fine-grained I/O control:</b> PADLL classifies, differentiates and controls requests at different levels of granularity, including operation type (*open*, *read*, *mkdir*), operation class (*metadata*, *data*, *extended attributes*), user, and job.
* <b>Global visibility:</b> PADLL ensures holistic control of all I/O workflows and coordinated access to the file system, preventing contention and unfair usage or shared storage resources. This is achieved using [Cheferd](https://github.com/dsrhaslab/cheferd), a hierarchical Software-Defined Storage control plane.
* <b>Custom QoS specification:</b> PADLL enables system administrators to create custom QoS policies for rate limiting jobs running at the cluster (e.g., uniform and priorty-based rate distribution, proportion sharing, DRF), protecting the file system from greedy jobs and I/O burstiness.

The storage middleware follows a [Software-Defined Storage](https://dl.acm.org/doi/10.1145/3385896?cid=99659535288) approach, being composed of two main components:
* <b>Data plane (PADLL):</b> The data plane is a multi-stage component that provides the building blocks for differentiating and rate limiting I/O workflows. The data plane corresponds to **this repository**.
* <b>Control plane (Cheferd):</b> The control plane is a global coordinator that manages all data plane stages to ensure that storage QoS policies are met over time and adjusted according to workload variations. The control plane repository can be found at [dsrhaslab/cheferd](https://github.com/dsrhaslab/cheferd).


Please cite the following paper if you use PADLL:

**Taming Metadata-intensive HPC Jobs Through Dynamic, Application-agnostic QoS Control**.
Ricardo Macedo, Mariana Miranda, Yusuke Tanimura, Jason Haga, Amit Ruhela, Stephen Lien Harrell, Richard Todd Evans, José Pereira, João Paulo.
*23rd IEEE/ACM International Symposium on Cluster, Cloud and Internet Computing (CCGrid 2023)*.

```bibtex
@inproceedings {Macedo2023Padll,
    title     = {Taming Metadata-intensive HPC Jobs Through Dynamic, Application-agnostic QoS Control},
    author    = {Ricardo Macedo and Mariana Miranda and Yusuke Tanimura and Jason Haga and Amit Ruhela and Stephen Lien Harrell and Richard Todd Evans and Jos{\'e} Pereira and Jo{\~a}o Paulo},
    booktitle = {23rd IEEE/ACM International Symposium on Cluster, Cloud and Internet Computing},
    year      = {2023}
}
```



## Getting started with padll
 
Tutorial to install and test padll.

<b>0. Install dependencies</b>
```shell
# Install PAIO data plane framework
$ cd /path/to/dir   # select the path to clone the PAIO github repository
$ git clone https://github.com/dsrhaslab/paio.git	# update to dsrhaslab/paio
$ cd paio
$ git checkout <set-release>
$ mkdir build; cd build
$ cmake ..; cmake --build .
$ export CPATH="/path/to/dir/paio/include:${CPATH}"
```

<b>1. Set up padll</b>
```shell
$ cd padll; mkdir build; cd build;
$ cmake ..
$ cmake --build .
$ export PATH_PADLL=$PWD
```

<b>2. Setup microbenchmark</b>
```shell
$ cd padll/tests/posix
$ g++ simple_test.cpp -o simple_test
$ cd ..
```

<b>3. Run microbenchmark</b>
```shell
$ cd padll
$ LD_PRELOAD=$PATH_PADLL/libpadll.so ./tests/posix/simple_test
```

## Features
...

## Contents
...

## System Requirements
...

## Dependencies
* `C++17`
* `g++ 9.3.0`
* `cmake 3.19.0-rc1`
* library dependencies are handled in `CMakeLists.txt`

...


## Acknowledgments
>We thank the [National Institute of Advanced Industrial Science and Technologies (AIST)](https://www.aist.go.jp/index_en.html)
for providing access to computational resources of [AI Bridging Cloud Infrastructure (ABCI)](https://abci.ai/).
>Work realized within the scope of the project [BigHPC](https://bighpc.wavecom.pt)
(POCI-01-0247-FEDER-045924), European Regional Development Fund, through the Operational Programme for Competitiveness and 
Internationalisation - COMPETE 2020 Programme under the Portugal 2020 Partnership Agreement, and by National Funds through the 
FCT - Portuguese Foundation for Science and Technology, I.P. on the scope of the UT Austin Portugal Program; PhD Fellowships 
SFRH/BD/146059/2019 and PD/BD/151403/2021; and the UT Austin-Portugal Program, a collaboration between the Portuguese Foundation 
of Science and Technology and the University of Texas at Austin, award UTA18-001217.

<p align="center">
    <img src=".media/main_page/fct-logo.png" width="60">
    <img src=".media/main_page/utaustin-portugal-logo.png" width="160">
    <img src=".media/main_page/aist-logo.gif" width="160">
</p>


## Contact
Please contact us at `ricardo.g.macedo@inesctec.pt` or `jtpaulo@inesctec.pt` with any questions.