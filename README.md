# COGENT

## About the software

**COGENT** is a console software tool aimed to help IC designers who work with HDL code to generate thir documentation from the HDL source files.

For the code listed below see the result .

```v
/** @top
sigmo super mega module
(antisigmo)
**/

module paral_sh_reg #(
    parameter WIDTH = 32,
    parameter IN_WIDTH = 4
)
(
    input wire clk,
    input wire Sclk,
    input wire reset,
    input wire signal,
    ...
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
| `No`     | `-f`  |`--format`| The format of the result document. Possible values will be `html` and `md`.<br/>By now only the `html` value is supported.| `--format <html\|md>`|
| `No`     | `-h`  |`--help`  | The help message will be shown. | `--path` |

