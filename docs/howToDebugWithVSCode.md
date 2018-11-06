# How to debug C++ with VS Code

VS Code is a great tool to develop code in many languages. This document is to tell you how to debug C++ addon with the VS Code.

### 1. Setting up VSCode debug

To Set up the VSCode debug you have two ways to achieve:

Option 1: Press Cmd + Shift + P to open the command bar, and type open `open launch.json`, choose `C++`.

Option 2: Click the debug button on the right sidebar, and Click the ⚙ button on the right top, choose `C++`.

![howtodebug](./image/howtodebug1.png)

In the `launch.json` you can see some template, like this:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(lldb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "enter program name, for example ${workspaceFolder}/a.out",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "lldb"
        }
    ]
}
```

`launch.json` defined which program you want to run after you click the run debug button, let's say we want run `node test/vtshaver.test.js`(In this command we want to use `node` to run `test/vtshaver.test.js`), so we need to change the configurations, put `program` to node's absolute path(you can use `which node` to find the path of current version of node), and change the `args` to `test/vtshaver.test.js`'s absolute path. 

Additional we want build C++ everytime before we start debug, so we add `preLaunchTask` into the config:

```
"preLaunchTask": "npm: build:dev",
```

Untill now, the config file could looks like this:


```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(lldb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "preLaunchTask": "npm: build:dev",
            "program": "/Users/mapbox-mofei/.nvm/versions/node/v8.11.3/bin/node",
            "args": ["/Users/mapbox-mofei/dev/mapbox/vtshaver/test/vtshaver.test.js"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "lldb"
        }
    ]
}
```

Now you can open any C++ files and click the left side of the line number to add a breakpoint, then go to the debug button on the sidebar, click the run button on the top. 

![howtodebug](./image/howtodebug2.png)

Now everything is done! Debug is Done! The program will stop at the breakpoint, you can use your mouse to explore the variable. If you want you program to continue from the breakpoint, you can navigate the program use the top control box. 

![howtodebug](./image/howtodebug3.png)
