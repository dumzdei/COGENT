`timescale 1 ns/1ps
module paral_sh_reg #(
    parameter WIDTH = 32,
    parameter IN_WIDTH = 4
)
(
    input wire clk,
    input wire Sclk,
    input wire reset,
    input wire input_signal,
    output wire out0,
    output wire out1,
    output wire out2,
    output wire out3,
    output wire error,
    output wire [WIDTH-1:0] data_out
);

function integer log2;
    input integer b;
    begin
        b = b - 1;
        for (log2 = 0; b > 0; log2 = log2 + 1) begin
            b = b >> 1;
        end
    end
endfunction

    wire [3:0] reg_data;

    reg rst_sync1, rst_sync2;
    reg Srst_sync, Srst_sync1, Srst_sync2;

    always @(posedge clk or negedge reset) begin
        if (!reset) begin
            rst_sync1 <= 1'b0;
            rst_sync2 <= 1'b0;
        end else begin
            rst_sync1 <= 1'b1;
            rst_sync2 <= rst_sync1;
        end
    end


    always @(posedge Sclk or negedge reset) begin
        if (!reset) begin
            Srst_sync1 <= 1'b0;
            Srst_sync2 <= 1'b0;
        end else begin
            Srst_sync1 <= 1'b1;
            Srst_sync2 <= Srst_sync1;
        end
    end


    sh_reg S(
        .clk(clk),
        .rst(rst_sync2),
        .input_signal(input_signal),
        .out0(out0),
        .out1(out1),
        .out2(out2),
        .out3(out3),
        .error(error),
        .reg_data(reg_data)
    );
/*
    paral_reg P(
        .Sclk(Sclk),
        .clk(clk),
        .reset(Srst_sync2),
        .rst(rst_sync2),
        .reg_data(reg_data),
        .data_out(data_out)
    );
*/

    reg [WIDTH-1:0] data;

    always @(posedge clk or negedge rst_sync2) begin
        if (!rst_sync2) begin
            data <= 0;
        end else begin
            data <= {data[(WIDTH-1)-(IN_WIDTH-1):0], reg_data};
        end
    end

    reg we;
    reg [log2(WIDTH/IN_WIDTH):0]counter;

    always @(posedge clk or negedge rst_sync2) begin
        if (!rst_sync2) begin
            we <= 0;
            counter <= 0;
        end else begin
            counter <= counter + 1;
            if (counter == WIDTH/IN_WIDTH-1) begin
                we <= 1;
            end
        end
    end

    always @(posedge Sclk or negedge Srst_sync2) begin
        if (!Srst_sync2) begin
            Srst_sync <= 0;
        end else begin
            Srst_sync <= ~Srst_sync2;
        end
    end



    fifo_gray_p2p F(
        .clk_r(Sclk),
        .clk_w(clk),
        .clr(Srst_sync),
        .re(1),
        .rst(reset),
        .we(we),
        .din(data),
        .dout(data_out)
    );

endmodule
