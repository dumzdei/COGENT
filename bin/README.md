# Module Documentation

## Table of Contents

- [simple_counter](#simple_counter)
- [fifo_gray_p2p](#fifo_gray_p2p)
- [fifo_pgray](#fifo_pgray)
- [fifo_s](#fifo_s)
- [paral_sh_reg](#paral_sh_reg) <sup><code>TOP</code></sup>
- [sh_reg](#sh_reg)

---

<a name="simple_counter"></a>
# simple_counter

**File:** `tests_verilog\counter.v`

---

## Module Diagram

<div align="center">

<svg width="800" height="330" viewBox="0 0 800 330" xmlns="http://www.w3.org/2000/svg">
<rect class="module-box" x="200" y="10" width="400" height="300"/>
<text class="module-name" x="400" y="40" text-anchor="middle">simple_counter</text>
<line class="input-port" x1="200" y1="60" x2="180" y2="60"/>
<circle class="port-circle input-port" cx="180" cy="60" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="55" text-anchor="end">clk</text>
<text class="port-text port-type-text" x="205" y="64">1 wire</text>
<line class="input-port" x1="200" y1="90" x2="180" y2="90"/>
<circle class="port-circle input-port" cx="180" cy="90" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="85" text-anchor="end">rst_n</text>
<text class="port-text port-type-text" x="205" y="94">1 wire</text>
<line class="output-port" x1="600" y1="60" x2="620" y2="60"/>
<circle class="port-circle output-port" cx="620" cy="60" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="55" text-anchor="start">q</text>
<text class="port-text port-type-text" x="595" y="64" text-anchor="end">3:0 reg</text>
</svg>


</div>

---

## Module Information

| Property | Value |
|----------|-------|
| **Status** | **[COMPLETE]** Verified in simulation |
| **Author** | Nikolaenkov Dmitry. Email: dimanik116@gmail.com |
| **Date** | 11.11.2025 |

---

## Description

4-bit up-counter with synchronous reset <br> This module implements a basic 4-bit binary counter that increments on every positive clock edge. The counter wraps around from 15 to 0 automatically. <br> A synchronous active-low reset allows initialization to zero when rst_n is low.

---

## Description

Clock, signal

---

## Description

Active-low synchronous reset

---

## Description

4-bit counter output

---

## Ports

| Name | Direction | Type | Width | Description |
|------|-----------|------|-------|-------------|
| `clk` | -> input | wire | 1 | Clock, signal |
| `rst_n` | -> input | wire | 1 | Active-low synchronous reset |
| `q` | <- output | reg | 3:0 | 4-bit counter output |

---

## Notes

> **Note:** This counter uses synchronous reset which provides  better timing closure and predictability. <br> The initial block ensures simulation starts from known state but  may not synthesize to hardware initialization in all tools.

---

## Warnings

> **Warning:** If rst_n is not properly synchronized to clk, <br> metastability issues may occur when reset is released near clock edge.

---


<div style="page-break-after: always;"></div>

<a name="fifo_gray_p2p"></a>
# fifo_gray_p2p

**File:** `tests_verilog\fifo_gray_p2p.v`

---

## Module Diagram

<div align="center">

<svg width="800" height="330" viewBox="0 0 800 330" xmlns="http://www.w3.org/2000/svg">
<rect class="module-box" x="200" y="10" width="400" height="300"/>
<text class="module-name" x="400" y="40" text-anchor="middle">fifo_gray_p2p</text>
<line class="input-port" x1="200" y1="60" x2="180" y2="60"/>
<circle class="port-circle input-port" cx="180" cy="60" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="55" text-anchor="end">clk_r</text>
<text class="port-text port-type-text" x="205" y="64">1 logic</text>
<line class="input-port" x1="200" y1="90" x2="180" y2="90"/>
<circle class="port-circle input-port" cx="180" cy="90" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="85" text-anchor="end">clk_w</text>
<text class="port-text port-type-text" x="205" y="94">1 logic</text>
<line class="input-port" x1="200" y1="120" x2="180" y2="120"/>
<circle class="port-circle input-port" cx="180" cy="120" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="115" text-anchor="end">clr</text>
<text class="port-text port-type-text" x="205" y="124">1 logic</text>
<line class="input-port" x1="200" y1="150" x2="180" y2="150"/>
<circle class="port-circle input-port" cx="180" cy="150" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="145" text-anchor="end">re</text>
<text class="port-text port-type-text" x="205" y="154">1 logic</text>
<line class="input-port" x1="200" y1="180" x2="180" y2="180"/>
<circle class="port-circle input-port" cx="180" cy="180" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="175" text-anchor="end">rst</text>
<text class="port-text port-type-text" x="205" y="184">1 logic</text>
<line class="input-port" x1="200" y1="210" x2="180" y2="210"/>
<circle class="port-circle input-port" cx="180" cy="210" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="205" text-anchor="end">we</text>
<text class="port-text port-type-text" x="205" y="214">1 logic</text>
<line class="input-port" x1="200" y1="240" x2="180" y2="240"/>
<circle class="port-circle input-port" cx="180" cy="240" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="235" text-anchor="end">din</text>
<text class="port-text port-type-text" x="205" y="244">DW-1:0 logic</text>
<line class="output-port" x1="600" y1="60" x2="620" y2="60"/>
<circle class="port-circle output-port" cx="620" cy="60" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="55" text-anchor="start">empty</text>
<text class="port-text port-type-text" x="595" y="64" text-anchor="end">1 logic</text>
<line class="output-port" x1="600" y1="90" x2="620" y2="90"/>
<circle class="port-circle output-port" cx="620" cy="90" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="85" text-anchor="start">full</text>
<text class="port-text port-type-text" x="595" y="94" text-anchor="end">1 logic</text>
<line class="output-port" x1="600" y1="120" x2="620" y2="120"/>
<circle class="port-circle output-port" cx="620" cy="120" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="115" text-anchor="start">diff_rd</text>
<text class="port-text port-type-text" x="595" y="124" text-anchor="end">AWg:0 logic</text>
<line class="output-port" x1="600" y1="150" x2="620" y2="150"/>
<circle class="port-circle output-port" cx="620" cy="150" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="145" text-anchor="start">diff_wr</text>
<text class="port-text port-type-text" x="595" y="154" text-anchor="end">AWg:0 logic</text>
<line class="output-port" x1="600" y1="180" x2="620" y2="180"/>
<circle class="port-circle output-port" cx="620" cy="180" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="175" text-anchor="start">dout</text>
<text class="port-text port-type-text" x="595" y="184" text-anchor="end">DW-1:0 logic</text>
</svg>


</div>

---

## Description

test

---

## Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `DW` | parameter | `32` |  |
| `AWg` | parameter | `4` |  |
| `AWs` | parameter | `2` |  |

---

## Ports

| Name | Direction | Type | Width | Description |
|------|-----------|------|-------|-------------|
| `clk_r` | -> input | logic | 1 |  |
| `clk_w` | -> input | logic | 1 |  |
| `clr` | -> input | logic | 1 |  |
| `re` | -> input | logic | 1 |  |
| `rst` | -> input | logic | 1 |  |
| `we` | -> input | logic | 1 |  |
| `din` | -> input | logic | DW-1:0 |  |
| `empty` | <- output | logic | 1 |  |
| `full` | <- output | logic | 1 |  |
| `diff_rd` | <- output | logic | AWg:0 |  |
| `diff_wr` | <- output | logic | AWg:0 |  |
| `dout` | <- output | logic | DW-1:0 |  |

---


<div style="page-break-after: always;"></div>

<a name="fifo_pgray"></a>
# fifo_pgray

**File:** `tests_verilog\fifo_pgray.v`

---

## Module Diagram

<div align="center">

<svg width="800" height="360" viewBox="0 0 800 360" xmlns="http://www.w3.org/2000/svg">
<rect class="module-box" x="200" y="10" width="400" height="330"/>
<text class="module-name" x="400" y="40" text-anchor="middle">fifo_pgray</text>
<line class="input-port" x1="200" y1="60" x2="180" y2="60"/>
<circle class="port-circle input-port" cx="180" cy="60" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="55" text-anchor="end">clk_r</text>
<text class="port-text port-type-text" x="205" y="64">1 logic</text>
<line class="input-port" x1="200" y1="90" x2="180" y2="90"/>
<circle class="port-circle input-port" cx="180" cy="90" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="85" text-anchor="end">clk_w</text>
<text class="port-text port-type-text" x="205" y="94">1 logic</text>
<line class="input-port" x1="200" y1="120" x2="180" y2="120"/>
<circle class="port-circle input-port" cx="180" cy="120" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="115" text-anchor="end">clr</text>
<text class="port-text port-type-text" x="205" y="124">1 logic</text>
<line class="input-port" x1="200" y1="150" x2="180" y2="150"/>
<circle class="port-circle input-port" cx="180" cy="150" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="145" text-anchor="end">re</text>
<text class="port-text port-type-text" x="205" y="154">1 logic</text>
<line class="input-port" x1="200" y1="180" x2="180" y2="180"/>
<circle class="port-circle input-port" cx="180" cy="180" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="175" text-anchor="end">rst</text>
<text class="port-text port-type-text" x="205" y="184">1 logic</text>
<line class="input-port" x1="200" y1="210" x2="180" y2="210"/>
<circle class="port-circle input-port" cx="180" cy="210" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="205" text-anchor="end">we</text>
<text class="port-text port-type-text" x="205" y="214">1 logic</text>
<line class="input-port" x1="200" y1="240" x2="180" y2="240"/>
<circle class="port-circle input-port" cx="180" cy="240" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="235" text-anchor="end">din</text>
<text class="port-text port-type-text" x="205" y="244">DW-1:0 logic</text>
<line class="output-port" x1="600" y1="60" x2="620" y2="60"/>
<circle class="port-circle output-port" cx="620" cy="60" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="55" text-anchor="start">empty</text>
<text class="port-text port-type-text" x="595" y="64" text-anchor="end">1 logic</text>
<line class="output-port" x1="600" y1="90" x2="620" y2="90"/>
<circle class="port-circle output-port" cx="620" cy="90" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="85" text-anchor="start">full</text>
<text class="port-text port-type-text" x="595" y="94" text-anchor="end">1 logic</text>
<line class="output-port" x1="600" y1="120" x2="620" y2="120"/>
<circle class="port-circle output-port" cx="620" cy="120" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="115" text-anchor="start">diff_rd</text>
<text class="port-text port-type-text" x="595" y="124" text-anchor="end">AW:0 logic</text>
<line class="output-port" x1="600" y1="150" x2="620" y2="150"/>
<circle class="port-circle output-port" cx="620" cy="150" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="145" text-anchor="start">diff_wr</text>
<text class="port-text port-type-text" x="595" y="154" text-anchor="end">AW:0 logic</text>
<line class="output-port" x1="600" y1="180" x2="620" y2="180"/>
<circle class="port-circle output-port" cx="620" cy="180" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="175" text-anchor="start">dout</text>
<text class="port-text port-type-text" x="595" y="184" text-anchor="end">DW-1:0 logic</text>
<line class="input-port" x1="200" y1="270" x2="180" y2="270"/>
<circle class="port-circle input-port" cx="180" cy="270" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="265" text-anchor="end">b</text>
<text class="port-text port-type-text" x="205" y="274">AW:0 logic</text>
<line class="input-port" x1="200" y1="300" x2="180" y2="300"/>
<circle class="port-circle input-port" cx="180" cy="300" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="295" text-anchor="end">g</text>
<text class="port-text port-type-text" x="205" y="304">AW:0 logic</text>
</svg>


</div>

---

## Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `DW` | parameter | `2` |  |
| `AW` | parameter | `5` |  |
| `R_EMPTY` | parameter | `0` |  |
| `R_FULL` | parameter | `0` |  |
| `R_DAT` | parameter | `0` |  |
| `MS` | localparam | `1` |  |

---

## Ports

| Name | Direction | Type | Width | Description |
|------|-----------|------|-------|-------------|
| `clk_r` | -> input | logic | 1 |  |
| `clk_w` | -> input | logic | 1 |  |
| `clr` | -> input | logic | 1 |  |
| `re` | -> input | logic | 1 |  |
| `rst` | -> input | logic | 1 |  |
| `we` | -> input | logic | 1 |  |
| `din` | -> input | logic | DW-1:0 |  |
| `empty` | <- output | logic | 1 |  |
| `full` | <- output | logic | 1 |  |
| `diff_rd` | <- output | logic | AW:0 |  |
| `diff_wr` | <- output | logic | AW:0 |  |
| `dout` | <- output | logic | DW-1:0 |  |
| `b` | -> input | logic | AW:0 |  |
| `g` | -> input | logic | AW:0 |  |

---


<div style="page-break-after: always;"></div>

<a name="fifo_s"></a>
# fifo_s

**File:** `tests_verilog\fifo_s.v`

---

## Module Diagram

<div align="center">

<svg width="800" height="630" viewBox="0 0 800 630" xmlns="http://www.w3.org/2000/svg">
<rect class="module-box" x="200" y="10" width="400" height="600"/>
<text class="module-name" x="400" y="40" text-anchor="middle">fifo_s</text>
<line class="input-port" x1="200" y1="60" x2="180" y2="60"/>
<circle class="port-circle input-port" cx="180" cy="60" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="55" text-anchor="end">clk</text>
<text class="port-text port-type-text" x="205" y="64">1 logic</text>
<line class="input-port" x1="200" y1="90" x2="180" y2="90"/>
<circle class="port-circle input-port" cx="180" cy="90" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="85" text-anchor="end">rst_n</text>
<text class="port-text port-type-text" x="205" y="94">1 logic</text>
<line class="input-port" x1="200" y1="120" x2="180" y2="120"/>
<circle class="port-circle input-port" cx="180" cy="120" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="115" text-anchor="end">clr</text>
<text class="port-text port-type-text" x="205" y="124">1 logic</text>
<line class="input-port" x1="200" y1="150" x2="180" y2="150"/>
<circle class="port-circle input-port" cx="180" cy="150" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="145" text-anchor="end">re</text>
<text class="port-text port-type-text" x="205" y="154">1 logic</text>
<line class="input-port" x1="200" y1="180" x2="180" y2="180"/>
<circle class="port-circle input-port" cx="180" cy="180" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="175" text-anchor="end">we</text>
<text class="port-text port-type-text" x="205" y="184">1 logic</text>
<line class="input-port" x1="200" y1="210" x2="180" y2="210"/>
<circle class="port-circle input-port" cx="180" cy="210" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="205" text-anchor="end">din</text>
<text class="port-text port-type-text" x="205" y="214">1 logic</text>
<line class="input-port" x1="200" y1="240" x2="180" y2="240"/>
<circle class="port-circle input-port" cx="180" cy="240" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="235" text-anchor="end">dout</text>
<text class="port-text port-type-text" x="205" y="244">1 logic</text>
<line class="input-port" x1="200" y1="270" x2="180" y2="270"/>
<circle class="port-circle input-port" cx="180" cy="270" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="265" text-anchor="end">empty</text>
<text class="port-text port-type-text" x="205" y="274">1 logic</text>
<line class="input-port" x1="200" y1="300" x2="180" y2="300"/>
<circle class="port-circle input-port" cx="180" cy="300" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="295" text-anchor="end">full</text>
<text class="port-text port-type-text" x="205" y="304">1 logic</text>
<line class="input-port" x1="200" y1="330" x2="180" y2="330"/>
<circle class="port-circle input-port" cx="180" cy="330" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="325" text-anchor="end">diff</text>
<text class="port-text port-type-text" x="205" y="334">1 logic</text>
<line class="input-port" x1="200" y1="360" x2="180" y2="360"/>
<circle class="port-circle input-port" cx="180" cy="360" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="355" text-anchor="end">clk</text>
<text class="port-text port-type-text" x="205" y="364">1 logic</text>
<line class="input-port" x1="200" y1="390" x2="180" y2="390"/>
<circle class="port-circle input-port" cx="180" cy="390" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="385" text-anchor="end">rst_n</text>
<text class="port-text port-type-text" x="205" y="394">1 logic</text>
<line class="input-port" x1="200" y1="420" x2="180" y2="420"/>
<circle class="port-circle input-port" cx="180" cy="420" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="415" text-anchor="end">clr</text>
<text class="port-text port-type-text" x="205" y="424">1 logic</text>
<line class="input-port" x1="200" y1="450" x2="180" y2="450"/>
<circle class="port-circle input-port" cx="180" cy="450" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="445" text-anchor="end">din</text>
<text class="port-text port-type-text" x="205" y="454">DW-1:0 logic</text>
<line class="output-port" x1="600" y1="60" x2="620" y2="60"/>
<circle class="port-circle output-port" cx="620" cy="60" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="55" text-anchor="start">dout</text>
<text class="port-text port-type-text" x="595" y="64" text-anchor="end">DW-1:0 logic</text>
<line class="input-port" x1="200" y1="480" x2="180" y2="480"/>
<circle class="port-circle input-port" cx="180" cy="480" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="475" text-anchor="end">we</text>
<text class="port-text port-type-text" x="205" y="484">1 logic</text>
<line class="input-port" x1="200" y1="510" x2="180" y2="510"/>
<circle class="port-circle input-port" cx="180" cy="510" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="505" text-anchor="end">re</text>
<text class="port-text port-type-text" x="205" y="514">1 logic</text>
<line class="output-port" x1="600" y1="90" x2="620" y2="90"/>
<circle class="port-circle output-port" cx="620" cy="90" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="85" text-anchor="start">full</text>
<text class="port-text port-type-text" x="595" y="94" text-anchor="end">1 logic</text>
<line class="output-port" x1="600" y1="120" x2="620" y2="120"/>
<circle class="port-circle output-port" cx="620" cy="120" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="115" text-anchor="start">empty</text>
<text class="port-text port-type-text" x="595" y="124" text-anchor="end">1 logic</text>
<line class="output-port" x1="600" y1="150" x2="620" y2="150"/>
<circle class="port-circle output-port" cx="620" cy="150" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="145" text-anchor="start">diff</text>
<text class="port-text port-type-text" x="595" y="154" text-anchor="end">AW:0 logic</text>
<line class="input-port" x1="200" y1="540" x2="180" y2="540"/>
<circle class="port-circle input-port" cx="180" cy="540" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="535" text-anchor="end">b</text>
<text class="port-text port-type-text" x="205" y="544">AW:0 logic</text>
<line class="input-port" x1="200" y1="570" x2="180" y2="570"/>
<circle class="port-circle input-port" cx="180" cy="570" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="565" text-anchor="end">b</text>
<text class="port-text port-type-text" x="205" y="574">AW:0 logic</text>
</svg>


</div>

---

## Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `DW` | parameter | `32` |  |
| `AW` | parameter | `4` |  |
| `R_DAT` | parameter | `0` |  |

---

## Ports

| Name | Direction | Type | Width | Description |
|------|-----------|------|-------|-------------|
| `clk` | -> input | logic | 1 |  |
| `rst_n` | -> input | logic | 1 |  |
| `clr` | -> input | logic | 1 |  |
| `re` | -> input | logic | 1 |  |
| `we` | -> input | logic | 1 |  |
| `din` | -> input | logic | 1 |  |
| `dout` | -> input | logic | 1 |  |
| `empty` | -> input | logic | 1 |  |
| `full` | -> input | logic | 1 |  |
| `diff` | -> input | logic | 1 |  |
| `clk` | -> input | logic | 1 |  |
| `rst_n` | -> input | logic | 1 |  |
| `clr` | -> input | logic | 1 |  |
| `din` | -> input | logic | DW-1:0 |  |
| `dout` | <- output | logic | DW-1:0 |  |
| `we` | -> input | logic | 1 |  |
| `re` | -> input | logic | 1 |  |
| `full` | <- output | logic | 1 |  |
| `empty` | <- output | logic | 1 |  |
| `diff` | <- output | logic | AW:0 |  |
| `b` | -> input | logic | AW:0 |  |
| `b` | -> input | logic | AW:0 |  |

---


<div style="page-break-after: always;"></div>

<a name="paral_sh_reg"></a>
# paral_sh_reg <sup><code>TOP</code></sup>

**File:** `tests_verilog\paral_sh_reg.v`

---

## Module Diagram

<div align="center">

<svg width="800" height="330" viewBox="0 0 800 330" xmlns="http://www.w3.org/2000/svg">
<rect class="module-box" x="200" y="10" width="400" height="300"/>
<text class="module-name" x="400" y="40" text-anchor="middle">paral_sh_reg</text>
<line class="input-port" x1="200" y1="60" x2="180" y2="60"/>
<circle class="port-circle input-port" cx="180" cy="60" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="55" text-anchor="end">clk</text>
<text class="port-text port-type-text" x="205" y="64">1 wire</text>
<line class="input-port" x1="200" y1="90" x2="180" y2="90"/>
<circle class="port-circle input-port" cx="180" cy="90" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="85" text-anchor="end">Sclk</text>
<text class="port-text port-type-text" x="205" y="94">1 wire</text>
<line class="input-port" x1="200" y1="120" x2="180" y2="120"/>
<circle class="port-circle input-port" cx="180" cy="120" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="115" text-anchor="end">reset</text>
<text class="port-text port-type-text" x="205" y="124">1 wire</text>
<line class="input-port" x1="200" y1="150" x2="180" y2="150"/>
<circle class="port-circle input-port" cx="180" cy="150" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="145" text-anchor="end">signal</text>
<text class="port-text port-type-text" x="205" y="154">1 wire</text>
<line class="output-port" x1="600" y1="60" x2="620" y2="60"/>
<circle class="port-circle output-port" cx="620" cy="60" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="55" text-anchor="start">out0</text>
<text class="port-text port-type-text" x="595" y="64" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="90" x2="620" y2="90"/>
<circle class="port-circle output-port" cx="620" cy="90" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="85" text-anchor="start">out1</text>
<text class="port-text port-type-text" x="595" y="94" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="120" x2="620" y2="120"/>
<circle class="port-circle output-port" cx="620" cy="120" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="115" text-anchor="start">out2</text>
<text class="port-text port-type-text" x="595" y="124" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="150" x2="620" y2="150"/>
<circle class="port-circle output-port" cx="620" cy="150" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="145" text-anchor="start">out3</text>
<text class="port-text port-type-text" x="595" y="154" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="180" x2="620" y2="180"/>
<circle class="port-circle output-port" cx="620" cy="180" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="175" text-anchor="start">error</text>
<text class="port-text port-type-text" x="595" y="184" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="210" x2="620" y2="210"/>
<circle class="port-circle output-port" cx="620" cy="210" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="205" text-anchor="start">data_out</text>
<text class="port-text port-type-text" x="595" y="214" text-anchor="end">WIDTH-1:0 wire</text>
<line class="input-port" x1="200" y1="180" x2="180" y2="180"/>
<circle class="port-circle input-port" cx="180" cy="180" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="175" text-anchor="end">b</text>
<text class="port-text port-type-text" x="205" y="184">1 logic</text>
</svg>


</div>

---

## Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `WIDTH` | parameter | `32` | Widht |
| `IN_WIDTH` | parameter | `4` |  |

---

## Ports

| Name | Direction | Type | Width | Description |
|------|-----------|------|-------|-------------|
| `clk` | -> input | wire | 1 |  |
| `Sclk` | -> input | wire | 1 |  |
| `reset` | -> input | wire | 1 |  |
| `signal` | -> input | wire | 1 |  |
| `out0` | <- output | wire | 1 |  |
| `out1` | <- output | wire | 1 |  |
| `out2` | <- output | wire | 1 |  |
| `out3` | <- output | wire | 1 |  |
| `error` | <- output | wire | 1 |  |
| `data_out` | <- output | wire | WIDTH-1:0 |  |
| `b` | -> input | logic | 1 |  |

---


<div style="page-break-after: always;"></div>

<a name="sh_reg"></a>
# sh_reg

**File:** `tests_verilog\sh_reg.v`

---

## Module Diagram

<div align="center">

<svg width="800" height="330" viewBox="0 0 800 330" xmlns="http://www.w3.org/2000/svg">
<rect class="module-box" x="200" y="10" width="400" height="300"/>
<text class="module-name" x="400" y="40" text-anchor="middle">sh_reg</text>
<line class="input-port" x1="200" y1="60" x2="180" y2="60"/>
<circle class="port-circle input-port" cx="180" cy="60" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="55" text-anchor="end">clk</text>
<text class="port-text port-type-text" x="205" y="64">1 wire</text>
<line class="input-port" x1="200" y1="90" x2="180" y2="90"/>
<circle class="port-circle input-port" cx="180" cy="90" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="85" text-anchor="end">rst</text>
<text class="port-text port-type-text" x="205" y="94">1 wire</text>
<line class="input-port" x1="200" y1="120" x2="180" y2="120"/>
<circle class="port-circle input-port" cx="180" cy="120" r="3" fill="#0099ff"/>
<text class="port-text input-port-text" x="175" y="115" text-anchor="end">signal</text>
<text class="port-text port-type-text" x="205" y="124">1 wire</text>
<line class="output-port" x1="600" y1="60" x2="620" y2="60"/>
<circle class="port-circle output-port" cx="620" cy="60" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="55" text-anchor="start">out0</text>
<text class="port-text port-type-text" x="595" y="64" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="90" x2="620" y2="90"/>
<circle class="port-circle output-port" cx="620" cy="90" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="85" text-anchor="start">out1</text>
<text class="port-text port-type-text" x="595" y="94" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="120" x2="620" y2="120"/>
<circle class="port-circle output-port" cx="620" cy="120" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="115" text-anchor="start">out2</text>
<text class="port-text port-type-text" x="595" y="124" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="150" x2="620" y2="150"/>
<circle class="port-circle output-port" cx="620" cy="150" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="145" text-anchor="start">out3</text>
<text class="port-text port-type-text" x="595" y="154" text-anchor="end">1 wire</text>
<line class="output-port" x1="600" y1="180" x2="620" y2="180"/>
<circle class="port-circle output-port" cx="620" cy="180" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="175" text-anchor="start">error</text>
<text class="port-text port-type-text" x="595" y="184" text-anchor="end">1 reg</text>
<line class="output-port" x1="600" y1="210" x2="620" y2="210"/>
<circle class="port-circle output-port" cx="620" cy="210" r="3" fill="#0066cc"/>
<text class="port-text output-port-text" x="625" y="205" text-anchor="start">reg_data</text>
<text class="port-text port-type-text" x="595" y="214" text-anchor="end">3:0 reg</text>
</svg>


</div>

---

## Ports

| Name | Direction | Type | Width | Description |
|------|-----------|------|-------|-------------|
| `clk` | -> input | wire | 1 |  |
| `rst` | -> input | wire | 1 |  |
| `signal` | -> input | wire | 1 |  |
| `out0` | <- output | wire | 1 |  |
| `out1` | <- output | wire | 1 |  |
| `out2` | <- output | wire | 1 |  |
| `out3` | <- output | wire | 1 |  |
| `error` | <- output | reg | 1 |  |
| `reg_data` | <- output | reg | 3:0 |  |

---


---

*Documentation generated automatically by COGENT*
