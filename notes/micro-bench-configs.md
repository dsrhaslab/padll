# PAIO and PADLL Configurations and Rules for Micro-benchmarks

**General PAIO configurations:**
- option_default_enforcement_object_differentiation_operation_type : false
- option_default_enforcement_object_differentiation_operation_context : false
- option_default_channel_statistic_collection : true
- option_default_object_statistic_collection : false
- option_default_statistic_metric : StatisticMetric::throughput

### 1a: rate limiting per-request type
**Testing scenario:**
- trace replayer that replayes the ABCI's mdt-01 log
- replayes the most metadata-intensive operations: **open**, **close**, **getxattr**, **rename**
- **read** and **write** using ***IOR*** or ***dd*** (*dd* was used in the TBF paper)
- rate changes over time (different phases/steps: t(0) = 100 IOPS; t(20) = 500 IOPS; t(50) = 250 IOPS; ...)

**Request considerations:** 
- definition : posix
- operation type : open, close, getxattr, rename, read, write
- operation context : no_op

**PAIO configurations:**
- option_default_channel_differentiation_workflow : false
- option_default_channel_differentiation_operation_type : true
- option_default_channel_differentiation_operation_context : false
- option_default_statistic_classifier : ClassifierType::operation_type
- option_default_context_type : ContextType::POSIX

**PADLL configurations:**
- libc_calls::padll_intercept_open_var | padll_intercept_open : true
- libc_calls::padll_intercept_close : true
- libc_calls::padll_intercept_rename : true
- libc_calls::padll_intercept_getxattr : true
- libc_calls::padll_intercept_read : true
- libc_calls::padll_intercept_write : true
- rest of libc_calls : false

**Rules**
- number of channels : 6 (one per request type; just for simplicity in number of configurations files to create)
- enforcement object : DRL

---

### 1b: rate limiting per-request group
**Testing scenario:**
- different threads (4) replaying different parts of ABCI's mdt-01 log
- each thread replays the log of a single metadata operation (open, close, getxattr, rename)
- rate limit all metadata operations (group)
- rate changes over time (different phases/steps: t(0) = 100 IOPS; t(20) = 500 IOPS; t(50) = 250 IOPS; ...)

**Request considerations:**
- definition : POSIX_META
- operation type : open, close, getxattr, rename, ...
- operation context : meta_op

**PAIO configurations:**
- option_default_channel_differentiation_workflow : false
- option_default_channel_differentiation_operation_type : false
- option_default_channel_differentiation_operation_context : true
- option_default_statistic_classifier : ClassifierType::operation_context
- option_default_context_type : ContextType::POSIX_META

**PADLL configurations:**
- libc_calls::padll_intercept_open_var | padll_intercept_open : true
- libc_calls::padll_intercept_close : true
- libc_calls::padll_intercept_rename : true
- libc_calls::padll_intercept_getxattr : true
- rest of libc_calls : false

**Rules**
- number of channels : 1
- enforcement object : DRL

---

### 1c: rate limiting per-job
**Testing scenario:**
- trace replayer that replayes the ABCI's mdt-01 log
- each job replays the same as in **1b**
- jobs have different priorities, and are rate limited more aggressively or not

**Request considerations:**
- definition : POSIX_META
- operation type : open, close, getxattr, rename, ...
- operation context : meta_op

**PAIO configurations:**
- same as in 1b
- one data plane stage per-job

**PADLL configurations:**
- libc_calls::padll_intercept_open_var | padll_intercept_open : true
- libc_calls::padll_intercept_close : true
- libc_calls::padll_intercept_rename : true
- libc_calls::padll_intercept_getxattr : true
- rest of libc_calls : false

---

### 2: cross-applicability
**Testing scenario:**
- test PADLL over real applications, namely TensorFlow, LAMMPS, and OpenFoam
- TensorFlow: single-node, multi-threaded (interleaving); LeNet (stored in Lustre); N epochs
- LAMMPS: single-node, multi-process (???); compute-intensive model (check TACC paper on PEARC)
- OpenFoam: single-node, multi-process (???); compute-intensive model

**Request considerations:**
- definition : POSIX_META
- operation type : all metadata operations supported by PADLL
- operation context : meta_op

**PAIO configurations:**
- option_default_channel_differentiation_workflow : true
- option_default_channel_differentiation_operation_type : false
- option_default_channel_differentiation_operation_context : true
- option_default_statistic_classifier : ClassifierType::operation_context
- option_default_context_type : ContextType::POSIX_META

**PADLL configurations:**
- libc_calls::PosixMetadataCalls : true
- libc_calls::PosixExtendedAttributedCalls : true
- libc_calls::PosixDirectoryCalls : true
- rest of libc_calls : false

**Rules**
- number of channels : 4 (to enabled load balancing)
    - note: test first with a single channel, and check if PAIO/PADLL is not a bottleneck; if not, use a single channel instead of 4
- enforcement object : DRL

---



 


