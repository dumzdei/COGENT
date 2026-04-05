/////////////////////////////
//
//* @top Test example 1
//
//* @brief module for add/sub operation
//
//* @author krt2019@yandex.ru
//
/////////////////////////////

module add #(
    parameter int DATA_WIDTH = 32,

    //* derived_parameters

    parameter type data_t  = logic[DATA_WIDTH-1:0]
) (
    input   logic   clk,
    input   logic   reset_n,

    input   logic   op_type,    //* Type of iperation: 1 -> add, 0 -> sub

    input   data_t  arg1,
    input   data_t  arg2,

    output  data_t  res,
    output  logic   ov
);

    always_ff @(posedge clk) begin
        if(~reset_n) begin
            {ov, res} <= '0;
        end
        else begin
            {ov, res} <= op_type ? arg1 + arg2 : arg1 - arg2;
        end
    end

endmodule