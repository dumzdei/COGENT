/** @brief
Module: simple_counter
Description: 4-bit up-counter with synchronous reset
@author Dima
**/ 
//* @note test
module simple_counter (
    input wire clk,     //* Clock, signal
    input wire rst_n,    //* Active-low synchronous reset
    output reg [3:0] q //* 4-bit counter output
);
    
    initial begin
        q = 4'b0000;
    end

    /** Counter process
     *  This process increments the counter on each rising edge of clk.
     *  If rst_n is low, counter resets to 0.
    **/
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            q <= 4'b0000;
        end else begin
            q <= q + 1;
        end
    end

endmodule
