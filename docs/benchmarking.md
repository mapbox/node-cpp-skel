# Benchmarking

This project includes [bench tests](https://github.com/mapbox/node-cpp-skel/tree/master/bench) you can use to experiment with and measure performance. These bench tests hit the functions that [simulate expensive work being done in the threadpool](https://github.com/mapbox/node-cpp-skel/blob/master/src/object_async/hello_async.cpp#L121-L122). This is intended to model realworld use cases where you, as the developer, have expensive computation you'd like to dispatch to worker threads. Adapt these tests to your custom code and monitor the performance of your code. 

We've included two bench tests for the async examples, demonstrating the affects of concurrency and threads within a process. For example, you can run:

```
node bench/hello_async.bench.js --iterations 50 --concurrency 10 --mem
```

This will run a bunch of calls to the module's `helloAsync()` function. You can control two things:

- iterations: number of times to call `helloAsync()`
- concurrency: max number of threads the test can utilize, by setting `UV_THREADPOOL_SIZE`. When running the bench script, you can see this number of threads reflected in your [Activity Monitor](https://github.com/springmeyer/profiling-guide#activity-monitorapp-on-os-x)/[htop window](https://hisham.hm/htop/). 
- `--mem`: Optional arg to show memory stats per [`process.memoryUsage()`](https://nodejs.org/api/process.html#process_process_memoryusage)

### Ideal Benchmarks

**Ideally, you want your workers to run your code ~99% of the time and never idle.** This reflects a healthy node c++ addon and what you would expect to see when you've picked a good problem to solve with node.

The bench tests and async functions that come with `node-cpp-skel` out of the box demonstrate this behaviour:
- An async function that is CPU intensive and takes a while to finish (expensive creation and querying of a `std::map` and string comparisons). 
- Worker threads are busy doing a lot of work, and the main loop is relatively idle. Depending on how many threads (concurrency) you enable, you may see your CPU% sky-rocket and your cores max out. Yeaahhh!!!
- If you bump up `--iterations` to 500 and [profile in Activity Monitor.app](https://github.com/springmeyer/profiling-guide#activity-monitorapp-on-os-x), you'll see the main loop is idle as expected since the threads are doing all the work. You'll also see the threads busy doing work in AsyncHelloWorker roughly 99% of the time :tada:

![](https://user-images.githubusercontent.com/1209162/29333300-e7c483e2-81c8-11e7-8253-1beb12173841.png)