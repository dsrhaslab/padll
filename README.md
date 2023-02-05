## PADLL: Programmable and Adaptable I/O for Dynamically Loaded Libraries

<h3 align="left">
  <!-- logo as of current commit -->
  <img src="https://img.shields.io/badge/C++-17-yellow.svg?style=flat&logo=c%2B%2B" />
  <img src="https://img.shields.io/badge/status-research%20prototype-green.svg" />
  <a href="https://opensource.org/licenses/BSD-3-Clause">
    <img src="https://img.shields.io/badge/license-BSD--3-blue.svg" />
  </a>
</h3>

PADLL is a storage middleware that enables system administrators to proactively and holistically control the rate of data and and metadata workflows to achieve QoS in HPC storage systems. 
Its design if build under the following core principles:

* <b>Application and file system agnostic:</b> PADLL does not require code changes to any core layer of the HPC I/O stack, being applicable over multiple applications and cross-compatible with POSIX-compliant file systems. PADLL achieves this using LD_PRELOAD.
* <b>Fine-grained I/O control:</b> PADLL classifies, differentiates and controls requests at different levels of granularity, including operation type (*open*, *read*, *mkdir*), operation class (*metadata*, *data*, *extended attributes*), user, and job.
* <b>Global visibility:</b> PADLL ensures holistic control of all I/O workflows and coordinated access to the file system, preventing contention and unfair usage or shared storage resources. This is achieved using [Cheferd](https://github.com/dsrhaslab/cheferd), a hierarchical Software-Defined Storage control plane.
* <b>Custom QoS specification:</b> PADLL enables system administrators to create custom QoS policies for rate limiting jobs running at the cluster (e.g., uniform and priorty-based rate distribution, proportion sharing, DRF), protecting the file system from greedy jobs and I/O burstiness.

The storage middleware follows a [Software-Defined Storage](https://dl.acm.org/doi/10.1145/3385896?cid=99659535288) approach, being composed of two main components:
* <b>Data plane (PADLL):</b> The data plane is a multi-stage component that provides the building blocks for differentiating and rate limiting I/O workflows. The data plane corresponds to [**this repository**](https://github.com/dsrhaslab/padll).
* <b>Control plane (Cheferd):</b> The control plane is a global coordinator that manages all data plane stages to ensure that storage QoS policies are met over time and adjusted according to workload variations. The control plane repository can be found at [dsrhaslab/cheferd](https://github.com/dsrhaslab/cheferd).


Please cite the following paper if you use PADLL:

>**Taming Metadata-intensive HPC Jobs Through Dynamic, Application-agnostic QoS Control**.
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

***

## Getting started with PADLL
 
This tutorial will guide on how to set up, benchmark, and use PADLL.

### Requirements and Dependencies
PADLL is written with C++17 and was built and tested with `g++-9.3.0` and `cmake-3.16`.
The core library was built using the [PAIO](https://github.com/dsrhaslab/paio) storage data plane framework (install instructions below).
It also uses the [spdlog v1.8.1](https://github.com/gabime/spdlog) logging library (installed at compile time).

Further, PADLL uses the following [third party]() libraries, which are embedded as single-header files: [Xoshiro-cpp](https://github.com/Reputeless/Xoshiro-cpp) (pseudorandom number generator library), [tabulate](https://github.com/p-ranav/tabulate) (library for printing aligned, formatted, and colorized tables), [better-enums](https://github.com/aantron/better-enums) (compile-time enum library).

<b>Install PAIO</b>

```shell
$ cd /path/to/dir   # select the path to clone the PAIO github repository
$ git clone https://github.com/dsrhaslab/paio.git
$ cd paio
$ git checkout v1.0.0
$ mkdir build; cd build
$ cmake ..; cmake --build .
$ export CPATH="/path/to/dir/paio/include:${CPATH}"
$ export LD_LIBRARY_PATH="/path/to/dir/paio/build/:${LD_LIBRARY_PATH}"
```

### Setup PADLL

```shell
$ cd /path/to/dir   # select the path to clone the PADLL github repository
$ $ git clone https://github.com/dsrhaslab/padll.git
$ cd padll
$ git checkout v1.0.0
$ mkdir build; cd build;
$ cmake ..
$ cmake --build .
$ export PATH_PADLL=$PWD
```

### Configuring and tuning PADLL
PADLL provides two sets of configurations:
* [options.hpp](https://github.com/dsrhaslab/padll/blob/master/include/padll/options/options.hpp): configurations related to the data plane stage are placed in the options header file.
* [libc_calls.hpp](https://github.com/dsrhaslab/padll/blob/master/include/padll/configurations/libc_calls.hpp): configurations related to which POSIX operations should be intercepted and handled with PADLL.

#### options header
```yaml
# include/padll/options/options.hpp

Mount point differentiation
- option_default_remote_mount_point : "/tmp"  # IMPORTANT: define the default mount point to control I/O requests

Statistic collection
- option_default_statistic_collection : true  # simple statistic collection to validate which requests were successfully handled

Data plane stage configuration
- option_default_stage_name : "padll-stage" # default name of the data plane stage
- option_paio_environment_variable_env : "paio_env" # environment variable to set additional information for the stage

Connection to control plane and standalone mode
- option_sync_with_controller : true  # IMPORTANT: defines if the data plane stage should execute standalone, or connect to the control plane  
- option_default_connection_address_env : "cheferd_local_address" # environment variable to define the connection address to the control plane local controller
- main_path : "/path/to/padll/files" # if running on standalone mode, define that path for housekeeping, differentiation, and enforcement files
- option_default_hsk_rules_file : "hsk-simple-test" # if running on standalone mode, define the path to the housekeeping rules file (will define PAIO's channels and enforcement objects)

Logging and Debugging
- option_default_log_path : "/tmp/padll-info" # default path for PADLL logging files
- option_default_statistics_report_path : "/tmp"  # main path to store statistic reports
- OPTION_DETAILED_LOGGING : false # detailed logging (mainly used for debugging)
```

#### libc calls header
```yaml
# include/padll/configurations/libc_calls.hpp

# to replicate the micro-benchmarking experiments of the PADLL paper (§5.1), 
# one should enable the following system calls.
- padll_intercept_open_var : true
- padll_intercept_open : true
- padll_intercept_close : true
- padll_intercept_rename : true
- padll_intercept_getxattr : true
- padll_intercept_read : true
- padll_intercept_write : true
# remainder configurations should be set at false
```

### Configuring and tuning PAIO

PADLL's core internals are built using the PAIO data plane framework, namely the request differentiation and rate limiting.
As such, depending on the use case that you may want to use PADLL, there are a few tuning knobs in PAIO that should be properly configured.
These configurations are in PAIO options header file ([options.hpp](https://github.com/dsrhaslab/paio/blob/main/include/paio/options/options.hpp)).

```yaml
# include/paio/options/options.hpp

Debugging and ld_preload settings
- option_default_debug_log : true   # minor logging messages do not introduce significant overhead
- option_default_ld_preload_enabled : true  # when using LD_PRELOAD, this should be set to prevent cyclic libc.so dependencies

Statistic collection
- option_default_channel_statistic_collection : true  # per-channel statistic collection
- option_default_statistic_metric : StatisticMetric::counter  # simple statistic counter (optionally use StatisticMetric::throughput)

Channel-level differentiation
- option_default_channel_differentiation_workflow : true  # classify requests based on their workflow-id
- option_default_channel_differentiation_operation_type : false # classify requests based on their operation type
- option_default_channel_differentiation_operation_context : false  # classify requests based on their operation context

Context identifier
- option_default_context_type : ContextType::POSIX  # class of operations to consider
- option_default_statistic_classifier : ClassifierType::operation_type  # type of classifier to be used on statistic counting
```


#### Simple test

```shell
$ cd padll/tests/posix
$ g++ simple_test.cpp -o simple_test
$ cd ..
```

#### PADLL scalability benchmark

```shell
$ cd /path/to/padll
$ LD_PRELOAD=$PATH_PADLL/libpadll.so ./build/padll_scalability_bench
```

#### PADLL paper experiments




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
