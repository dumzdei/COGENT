# COGENT

## About the software

**COGENT** is a console software tool aimed to help IC designers who work with HDL code to generate their documentation from the HDL source files.

For the code listed below see the result.

```v
/** @brief
4-bit up-counter with synchronous reset \
This module implements a basic 4-bit binary counter that increments on every
positive clock edge. The counter wraps around from 15 to 0 automatically. \
A synchronous active-low reset allows initialization to zero when rst_n is low.
*/

/** @note This counter uses synchronous reset which provides 
better timing closure and predictability. \
The initial block ensures simulation starts from known state but 
may not synthesize to hardware initialization in all tools.
*/

/** @warning If rst_n is not properly synchronized to clk, \
metastability issues may occur when reset is released near clock edge.
*/

/**
@status Verified in simulation
@author Nikolaenkov Dmitry. Email: dimanik116@gmail.com
@date 11.11.2025
*/
module simple_counter (
    input wire clk,     //* Clock, signal
    input wire rst_n,    //* Active-low synchronous reset
    output reg [3:0] q //* 4-bit counter output
);
...
endmodule
```

![An example of the output](screen.png)

## How to compile

#### Under MS Windows

For the MS Windows OS we provide the Visual Studio 2022 solution file. 
Just open the ``COGENT\msvs2022`` folder and load the ``COGENT.sln`` solution file.

#### Under Linux

For those who use Linux the Makefile is provided. Find it in the ``COGENT\build`` folder.

## How to run the software

To run the software you should the options as it shown in the table below:

| Required | Short | Full     | Description | Usage |
|  :---:   | :---: |   :---:  |    :---    | :---:  |
| `Yes`    | `-p`  |`--path`  | The path to the input files folder. | `--path <path>` |
| `No`     | `-s`  |`--style` | The style of the theme, possible values are: `light` and `dark`.<br/>By default the `dark` style is used. | `--style <light\|dark>`|
| `No`     | `-f`  |`--format`| The format of the result document. Possible values will be `html`, `markdown` and `asciidoc`. | `--format <html\|{md\|markdown}\|{adoc\|asciidoc}>`|
| `No`     | `-h`  |`--help`  | The help message will be shown. | `--help` |

