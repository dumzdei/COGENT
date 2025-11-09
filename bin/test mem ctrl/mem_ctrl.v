/** @brief 
Memory controller for working with cache and main RAM.
Interacts between the processor and memory,
ensuring data storage and retrieval through the cache.
**/
 
//* @author Dima

//* @date 2025-11-08
//* @top A memory controller that connects the CPU to the main memory and cache.
module memory_controller
#(
    parameter ADDR_WIDTH = 32,
    parameter DATA_WIDTH = 64,
    parameter CACHE_SIZE = 128
)
(
    //* @note The inputs and outputs are connected to the CPU and the data bus.
    input  wire                  clk,        //* Clock signal
    input  wire                  rst_n,      //* Reset (active low)
    input  wire [ADDR_WIDTH-1:0] addr_in,    //* Address entry
    input  wire [DATA_WIDTH-1:0] data_in,    //* Data entry
    output reg  [DATA_WIDTH-1:0] data_out,   //* Data output
    input  wire                  we,         //* Recording signal
    input  wire                  re,         //* Read signal
    output reg                   ready       //* Readiness flag
);

reg [DATA_WIDTH-1:0] cache [0:CACHE_SIZE-1];
reg [ADDR_WIDTH-1:0] tags_mem [0:CACHE_SIZE-1];
reg valid [0:CACHE_SIZE-1];
integer i;

//* @todo Implement more efficient cache mapping (associative)
//* @status in_progress

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ready <= 0;
        data_out <= 0;
        for (i = 0; i < CACHE_SIZE; i = i + 1) begin
            valid[i] <= 0;
        end
    end else begin
        ready <= 0;
        if (re) begin
            for (i = 0; i < CACHE_SIZE; i = i + 1) begin
                if (valid[i] && tags_mem[i] == addr_in) begin
                    data_out <= cache[i];
                    ready <= 1;
                end
            end
        end else if (we) begin
            cache[addr_in % CACHE_SIZE] <= data_in;
            tags_mem[addr_in % CACHE_SIZE] <= addr_in;
            valid[addr_in % CACHE_SIZE] <= 1;
            ready <= 1;
        end
    end
end
/** @example
Example usage:
addr_in = 32'h0040;
data_in = 64'hABCD1234EF00AA55;
we = 1;re = 0;
After the clock edge, the data will be written to the cache.
**/
endmodule