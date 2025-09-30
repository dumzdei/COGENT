`timescale 1 ns/1ps
module sh_reg(
    input wire clk,
    input wire rst,
    input wire input_signal,
    output wire out0,
    output wire out1,
    output wire out2,
    output wire out3,
    output reg error,
    output reg [3:0] reg_data
);
    //reg [3:0] reg_data;

    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            reg_data <= 4'b0000;
        end
        else begin
            reg_data <= {reg_data[2:0], input_signal};
        end
    end

    assign out0 = (reg_data == 4'b0000);
    assign out1 = (reg_data == 4'b0011);
    assign out2 = (reg_data == 4'b1100);
    assign out3 = (reg_data == 4'b1111);

    reg [3:0] counter;

    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            counter <= 4'b0000;
            error <= 0;
        end
        else if (out0) begin
            counter <= 4'b0000;
            error <= 0;
        end
        else if (counter >= 4'b1010) begin
            error <= 1;
        end
        else begin
            counter <= counter + 1;
        end
    end

endmodule
