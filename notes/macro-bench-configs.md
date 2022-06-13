# PAIO and PADLL Configurations and Rules for Macro-benchmarks

**General PAIO configurations:**
```yaml
EnforcementObject differentiation
- option_default_enforcement_object_differentiation_operation_type : false
- option_default_enforcement_object_differentiation_operation_context : false

Statistic collection
- option_default_channel_statistic_collection : true
- option_default_object_statistic_collection : false
- option_default_statistic_metric : StatisticMetric::throughput
```

### 3: QoS control of metadata-only jobs (synthetic)
**Testing scenario:**
- multiple jobs replaying the ABCI traces, one for each *mdt* log
- each job is multi-threaded, and each thread submits a specific operation type
- goal: rate limit all metadata operations of all jobs (group)
- different rate limiting scenarios:
    - (a) jobs are statically rate limited with the same rate;
    - (b) jobs are statically rate limited with different rates (priority);
    - (c) jobs are dynamically rate limited with different rates (QoS algorithm + priority).

**Request considerations:**
- definition : POSIX_META
- operation type : open, close, getxattr, rename, ...
- operation context : meta_op

**PAIO configurations:**
```yaml
Channel-level differentiation
- option_default_channel_differentiation_workflow : false
- option_default_channel_differentiation_operation_type : false
- option_default_channel_differentiation_operation_context : true

Context identifier
- option_default_context_type : ContextType::POSIX_META
- option_default_statistic_classifier : ClassifierType::operation_context
```

**PADLL configurations:**
```yaml
libc_calls header
- padll_intercept_open_var : true
- padll_intercept_open : true
- padll_intercept_close : true
- padll_intercept_rename : true
- padll_intercept_getxattr : true
- rest of libc_calls : false
```

**Rules (per data plane stage rules)**
- number of channels : 1
- enforcement object : DRL
- housekeeping rules file : `files/hsk-micro-2`

---

### 4: QoS control of (bursty) jobs (real use case)
**Testing scenario:**
- multiple TensorFlow jobs (4) training a dataset
- each TensorFlow job is distributed (8 workers), and multi-threaded (interleaving)
- goal: demonstrate that PADLL can control the burstiness of metadata operations, while ensuring QoS control of data operations as well (similar to PAIO's use case 2)
- each job runs with different demands of data operations, but all with the same demand for metadata
    - 4000 IOPS metadata : {job1, 1000} .. {job4, 1000}
    - 5GiB/s data (bandwidth) : {job1, 1.5GiB/s}, {job2, 0.5GiB/s}, {job3, 2GiB/s}, {job4, 1GiB/s}

**Request considerations:**
- definition : POSIX_META
- operation type : both data and metadata operations supported by PADLL
- operation context : meta_op, data_op

**PAIO configurations:**
```yaml
Channel-level differentiation # multiple channels to attend requests in parallel
- option_default_channel_differentiation_workflow : true
- option_default_channel_differentiation_operation_type : false
- option_default_channel_differentiation_operation_context : false

EnforcementObject-level differentiation # different objects to attend data and metadata requests
- option_default_enforcement_object_differentiation_operation_type : false
- option_default_enforcement_object_differentiation_operation_context : true

Context identifier
- option_default_context_type : ContextType::POSIX_META
- option_default_statistic_classifier : ClassifierType::operation_context
```

**PADLL configurations:**
```yaml
libc_calls header
- PosixMetadataCalls : true
- PosixExtendedAttributedCalls : true
- PosixDirectoryCalls : true
- read : true
- rest of libc_calls : false
```

**Rules**
- number of channels : 4 (to enabled load balancing)
    > **Note:** test first with a single channel, and check if PAIO/PADLL is not a bottleneck; if not, use a single channel instead of 4
- enforcement object : 2 DRL (one for `meta_op`, one for `data_op`)
- housekeeping rules file : `files/hsk-macro-1`

---

### 5: Preventing MDS/MDT overloading (real use case)
**Testing scenario:**
- multiple TensorFlow jobs (4) training a dataset
- each TensorFlow job is distributed (8 workers), and multi-threaded (interleaving)
- datasets are unbalanced, in terms of which MDT holds their metadata
- goal: demonstrate that PADLL can prevent MDS/MDT from overloading by controlling the amount of metadata operations submitted to each of them
- datasets are managed by different MDSs, and each MDS has access to the same amount of metadata operations
    - MDS1 : holds the dataset of {job1} .. {job3}
    - MDS2 : holds the dataset of {job4}

**Request considerations:**
- definition : POSIX_META
- operation type : metadata operations supported by PADLL
- operation context : meta_op

**PAIO configurations:**
```yaml
Channel-level differentiation # multiple channels to attend requests in parallel
- option_default_channel_differentiation_workflow : true
- option_default_channel_differentiation_operation_type : false
- option_default_channel_differentiation_operation_context : true

Context identifier
- option_default_context_type : ContextType::POSIX_META
- option_default_statistic_classifier : ClassifierType::operation_context
```

**PADLL configurations:**
```yaml
libc_calls header
- PosixMetadataCalls : true
- PosixExtendedAttributedCalls : true
- PosixDirectoryCalls : true
- rest of libc_calls : false
```

**Rules**
- number of channels : 4 (to enabled load balancing)
    > **Note:** test first with a single channel, and check if PAIO/PADLL is not a bottleneck; if not, use a single channel instead of 4
- enforcement object : DRL
- housekeeping rules file : `files/hsk-micro-3`

---

