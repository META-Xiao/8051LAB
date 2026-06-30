#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_8051_overview.py — Standard 8051 SFR Master Overview + Memory Map

Tight-layout charts with inline legends (like interrupt_priority.png style).
"""

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch
import os

OUT_DIR = "charts"
os.makedirs(OUT_DIR, exist_ok=True)

plt.rcParams.update({
    "font.family": "monospace",
    "font.size": 8,
    "figure.dpi": 200,
    "savefig.dpi": 200,
    "savefig.bbox": "tight",
    "savefig.pad_inches": 0.08,
})

# ═════════════════════════════════════════════════════════
SFR_TABLE = [
    ("CAT", "I/O Ports"),
    (0x80, "P0",  ["P0.7","P0.6","P0.5","P0.4","P0.3","P0.2","P0.1","P0.0"],
     [0x87,0x86,0x85,0x84,0x83,0x82,0x81,0x80], True,  0xFF, "Open-Drain\nNeeds pull-up"),
    (0x90, "P1",  ["P1.7","P1.6","P1.5","P1.4","P1.3","P1.2","P1.1","P1.0"],
     [0x97,0x96,0x95,0x94,0x93,0x92,0x91,0x90], True,  0xFF, "Internal\npull-up"),
    (0xA0, "P2",  ["P2.7","P2.6","P2.5","P2.4","P2.3","P2.2","P2.1","P2.0"],
     [0xA7,0xA6,0xA5,0xA4,0xA3,0xA2,0xA1,0xA0], True,  0xFF, "Int. pull-up\n+ High addr"),
    (0xB0, "P3",  ["P3.7","P3.6","P3.5","P3.4","P3.3","P3.2","P3.1","P3.0"],
     [0xB7,0xB6,0xB5,0xB4,0xB3,0xB2,0xB1,0xB0], True,  0xFF, "Int. pull-up\n+ Alt func."),

    ("CAT", "Interrupt Control"),
    (0xA8, "IE",  ["EA", "—",  "ET2","ES", "ET1","EX1","ET0","EX0"],
     [0xAF,None,0xAD,0xAC,0xAB,0xAA,0xA9,0xA8], True,  0x00, "Interrupt Enable\n0=Dis 1=En"),
    (0xB8, "IP",  ["—",  "—",  "PT2","PS", "PT1","PX1","PT0","PX0"],
     [None,None,0xBD,0xBC,0xBB,0xBA,0xB9,0xB8], True,  0x00, "Interrupt Priority\n0=Low 1=High"),

    ("CAT", "Timer / Counter"),
    (0x88, "TCON",["TF1","TR1","TF0","TR0","IE1","IT1","IE0","IT0"],
     [0x8F,0x8E,0x8D,0x8C,0x8B,0x8A,0x89,0x88], True,  0x00, "Timer Control\nRun/Stop/Flags"),
    (0x89, "TMOD",["GATE","C/T","M1","M0","GATE","C/T","M1","M0"],
     None, False, 0x00, "Timer Mode\nHi4=T1 Lo4=T0"),
    (0x8A, "TL0", ["Timer 0 Low Byte"],                   None, False, 0x00, None),
    (0x8B, "TL1", ["Timer 1 Low Byte"],                   None, False, 0x00, None),
    (0x8C, "TH0", ["Timer 0 High Byte"],                  None, False, 0x00, None),
    (0x8D, "TH1", ["Timer 1 High Byte"],                  None, False, 0x00, None),

    ("CAT", "Serial Port (UART)"),
    (0x98, "SCON",["SM0","SM1","SM2","REN","TB8","RB8","TI","RI"],
     [0x9F,0x9E,0x9D,0x9C,0x9B,0x9A,0x99,0x98], True,  0x00, "Serial Control\nMode / Flags"),
    (0x99, "SBUF",["Serial Data Buffer (Tx & Rx share 0x99)"],None,False,0x00,"Tx/Rx share\nsame addr"),

    ("CAT", "System Control"),
    (0xD0, "PSW", ["CY","AC","F0","RS1","RS0","OV","—","P"],
     [0xD7,0xD6,0xD5,0xD4,0xD3,0xD2,None,0xD0], True,  0x00, "Program Status\nWord (flags+bank)"),
    (0x81, "SP",  ["Stack Pointer  (reset = 0x07)"],      None, False, 0x07, "Stack Pointer\nPUSH: SP++"),
    (0x82, "DPL", ["Data Pointer Low byte"],               None, False, 0x00, None),
    (0x83, "DPH", ["Data Pointer High byte"],              None, False, 0x00, "DPTR = DPH:DPL"),
    (0x87, "PCON",["SMOD","—","—","—","GF1","GF0","PD","IDL"],
     None, False, 0x00, "Power Control\nSMOD=baud x2"),

    ("CAT", "Arithmetic"),
    (0xE0, "ACC", ["Accumulator  A  (all ALU results)"],   None, True,  0x00, "Accumulator\nCore ALU ops"),
    (0xF0, "B",   ["B Register  (MUL/DIV 2nd operand)"],   None, True,  0x00, "MUL: AxB -> B:A\nDIV: A/B -> A rem B"),

    ("CAT", "Timer 2  (8052 / AT89C52 only)"),
    (0xC8, "T2CON",["TF2","EXF2","RCLK","TCLK","EXEN2","TR2","C/T2","CP/RL2"],
     [0xCF,0xCE,0xCD,0xCC,0xCB,0xCA,0xC9,0xC8], True, 0x00, "T2 Control\n8052 only"),
    (0xC9, "T2MOD",["—","—","—","—","—","—","T2OE","DCEN"],None,False,0x00,"T2 Mode (8052)"),
    (0xCA, "RCAP2L",["T2 Capture/Reload Low"],              None, False, 0x00, "8052 only"),
    (0xCB, "RCAP2H",["T2 Capture/Reload High"],             None, False, 0x00, "8052 only"),
    (0xCC, "TL2", ["Timer 2 Low"],                          None, False, 0x00, "8052 only"),
    (0xCD, "TH2", ["Timer 2 High"],                         None, False, 0x00, "8052 only"),
]

# ═════════════════════════════════════════════════════════
#  Chart 1: SFR Master Overview
# ═════════════════════════════════════════════════════════

def draw_overview():
    rows_data = [r for r in SFR_TABLE if r[0] != "CAT"]
    categories = [r for r in SFR_TABLE if r[0] == "CAT"]
    data_rows = len(rows_data)
    cat_rows = len(categories)

    ROW_H    = 1.08
    CAT_H    = 0.65    # taller: gives breathing room above/below each category bar
    HEADER_H = 0.65
    TITLE_H  = 1.35
    LEGEND_H = 1.30    # two-row legend + footnote at bottom

    total_h = TITLE_H + HEADER_H + data_rows * ROW_H + cat_rows * CAT_H + LEGEND_H

    fig_w = 17.5
    fig, ax = plt.subplots(figsize=(fig_w, total_h * 0.62))
    ax.set_xlim(0, fig_w)
    ax.set_ylim(0, total_h)
    ax.set_aspect("equal")
    ax.axis("off")

    # Colors
    C_ADDR     = "#263238"
    C_BIT_A    = "#A5D6A7"
    C_BIT_N    = "#BBDEFB"
    C_SINGLE   = "#FFE082"
    C_RESV     = "#ECEFF1"
    C_8052     = "#E1BEE7"
    C_CAT      = "#37474F"
    C_RST      = "#FFF3E0"
    C_BITNUM   = "#BDBDBD"

    # Column layout — compressed
    x_addr  = 0.3
    x_name  = 2.1
    x_note  = 3.7
    x_bits  = 5.8
    bit_w   = 1.15
    bit_gap = 0.08
    pitch   = bit_w + bit_gap   # ≈1.23
    x_rst   = x_bits + 8 * pitch + 0.25

    # ── Title ──
    title_y = total_h - 0.25
    ax.text(fig_w / 2, title_y,
            "Standard MCS-51 (8051)  SFR  Register  Overview",
            ha="center", va="top", fontsize=14, fontweight="bold",
            fontfamily="monospace", color="#1A237E")
    ax.text(fig_w / 2, title_y - 0.45,
            "Bit 7 (MSB)  <--  ...  <--  Bit 0 (LSB)   |   Green = bit-addressable   |   Gray nums = bit addresses",
            ha="center", va="top", fontsize=7, fontfamily="monospace",
            color="#78909C", fontstyle="italic")

    # ── Header row ──
    header_y = title_y - TITLE_H
    ax.text(x_addr + 0.7, header_y + 0.15, "Addr", ha="center", va="center",
            fontsize=6.5, fontfamily="monospace", color="#90A4AE", fontweight="bold")
    ax.text(x_name + 0.7, header_y + 0.15, "SFR", ha="center", va="center",
            fontsize=6.5, fontfamily="monospace", color="#90A4AE", fontweight="bold")
    for b in range(8):
        bx = x_bits + b * pitch
        ax.text(bx + bit_w/2, header_y + 0.15, f"bit{7-b}", ha="center", va="center",
                fontsize=5, fontfamily="monospace", color="#90A4AE", fontweight="bold")
    ax.text(x_rst + 0.3, header_y + 0.15, "RST", ha="center", va="center",
            fontsize=6.5, fontfamily="monospace", color="#90A4AE", fontweight="bold")

    # ── Data rows ──
    cy = header_y - ROW_H * 0.35
    drawn = 0

    for item in SFR_TABLE:
        if item[0] == "CAT":
            # Center the bar within CAT_H space for even padding above & below
            bar_top    = cy
            bar_bottom = cy - CAT_H
            bar_mid    = (bar_top + bar_bottom) / 2
            bar_h      = 0.40
            bar = FancyBboxPatch((0.1, bar_mid - bar_h/2), fig_w - 0.2, bar_h,
                                  boxstyle="round,pad=0.05", facecolor=C_CAT,
                                  edgecolor="none", alpha=0.9, zorder=3)
            ax.add_patch(bar)
            ax.text(0.5, bar_mid, item[1], ha="left", va="center",
                    fontsize=9, fontfamily="monospace", color="white",
                    fontweight="bold", zorder=5)
            cy -= CAT_H
            continue

        addr, name, bits, bit_addrs, bit_addrbl, rst_val, note = item

        # Zebra background
        bg_c = "#FAFAFA" if drawn % 2 == 0 else "#FFFFFF"
        bg = FancyBboxPatch((0.05, cy - ROW_H + 0.04), fig_w - 0.1, ROW_H - 0.08,
                             boxstyle="round,pad=0.03", facecolor=bg_c,
                             edgecolor="#E0E0E0", linewidth=0.2, alpha=0.6)
        ax.add_patch(bg)

        byte_cy = cy - ROW_H * 0.48  # vertical center of the row

        # Address
        a_box = FancyBboxPatch((x_addr, byte_cy - 0.24), 1.45, 0.5,
                                boxstyle="round,pad=0.05", facecolor=C_ADDR,
                                edgecolor="none", alpha=0.88)
        ax.add_patch(a_box)
        ax.text(x_addr + 0.73, byte_cy + 0.01, f"0x{addr:02X}",
                ha="center", va="center", fontsize=8, fontfamily="monospace",
                color="white", fontweight="bold")

        # Name
        nc = "#2E7D32" if bit_addrbl else "#1565C0"
        ax.text(x_name + 0.7, byte_cy + 0.01, name, ha="center", va="center",
                fontsize=9.5, fontfamily="monospace", color=nc, fontweight="bold")

        # Note
        if note:
            ax.text(x_note + 0.2, byte_cy + 0.01, note, ha="left", va="center",
                    fontsize=5, fontfamily="monospace", color="#78909C", linespacing=1.15)

        # Bit cells
        if len(bits) == 1:
            bx = x_bits
            bw = 8 * pitch - bit_gap
            cc = C_8052 if addr >= 0xC8 else C_SINGLE
            rect = FancyBboxPatch((bx, byte_cy - 0.24), bw, 0.5,
                                   boxstyle="round,pad=0.06", facecolor=cc,
                                   edgecolor="#B0BEC5", linewidth=0.5)
            ax.add_patch(rect)
            ax.text(bx + bw/2, byte_cy + 0.01, bits[0], ha="center", va="center",
                    fontsize=6.5, fontfamily="monospace", color="#37474F", fontweight="bold")
        else:
            for b in range(8):
                bx = x_bits + b * pitch
                label = bits[7 - b]
                if label in ("—", ""):
                    cc, tc, fw, fs = C_RESV, "#B0BEC5", "normal", 6
                elif addr >= 0xC8:
                    cc, tc, fw, fs = C_8052, "#6A1B9A", "bold", 7.5
                elif bit_addrbl:
                    cc, tc, fw, fs = C_BIT_A, "#1B5E20", "bold", 7.5
                else:
                    cc, tc, fw, fs = C_BIT_N, "#0D47A1", "bold", 7.5

                rect = FancyBboxPatch((bx, byte_cy - 0.24), bit_w, 0.5,
                                       boxstyle="round,pad=0.05", facecolor=cc,
                                       edgecolor="#B0BEC5", linewidth=0.4)
                ax.add_patch(rect)
                ax.text(bx + bit_w/2, byte_cy + 0.01, label, ha="center", va="center",
                        fontsize=fs, fontfamily="monospace", color=tc, fontweight=fw)

                # Bit address below
                if bit_addrs is not None and bit_addrs[7 - b] is not None:
                    ax.text(bx + bit_w/2, byte_cy - 0.45,
                            f"{bit_addrs[7-b]:02X}H", ha="center", va="center",
                            fontsize=4.3, fontfamily="monospace", color=C_BITNUM)

        # Reset value
        rst_w = 0.7
        rst_box = FancyBboxPatch((x_rst, byte_cy - 0.24), rst_w, 0.5,
                                  boxstyle="round,pad=0.03", facecolor=C_RST,
                                  edgecolor="#FFCC80", linewidth=0.4)
        ax.add_patch(rst_box)
        ax.text(x_rst + rst_w/2, byte_cy + 0.01, f"{rst_val:02X}H",
                ha="center", va="center", fontsize=6.5, fontfamily="monospace",
                color="#E65100", fontweight="bold")

        if bit_addrbl:
            ax.text(x_rst + rst_w + 0.25, byte_cy + 0.01, "[B]", ha="left", va="center",
                    fontsize=5.5, fontfamily="monospace", color="#2E7D32", fontstyle="italic")

        cy -= ROW_H
        drawn += 1

    # ── Footnote (above legend) ──
    fn_y = cy + 0.15
    ax.text(fig_w / 2, fn_y,
            "8031 = ROMless   |   8051 = 4 KB ROM, 128 B RAM   |   8052 = 8 KB ROM, 256 B RAM, +Timer 2\n"
            "ET2 / PT2 in IE / IP are 8052 only.  Baud rate = (2^SMOD / 32) * (fosc / 12) / (256 - TH1).  "
            "11.0592 MHz crystal -> 0% error at all standard baud rates.",
            ha="center", va="top", fontsize=5.8, fontfamily="monospace", color="#90A4AE",
            linespacing=1.4, zorder=10)

    # ── Legend (two rows, at very bottom) ──
    leg_y_top = fn_y - 0.75
    leg_items = [
        (C_BIT_A,   "Bit-Addr bits"),  (C_BIT_N,   "No-Bit-Addr bits"),
        (C_SINGLE,  "Single-value"),    (C_8052,    "8052 only"),
        (C_RESV,    "Reserved"),        (C_RST,     "Reset value"),
        (C_BITNUM,  "Bit address num"),
    ]
    # Row 1: first 4 items, Row 2: last 3 items centered
    row1 = leg_items[:4]
    row2 = leg_items[4:]
    spacing = 3.6   # per item slot width
    for row_idx, row_items in enumerate([row1, row2]):
        n = len(row_items)
        total_w = n * spacing
        start_x = (fig_w - total_w) / 2
        row_y = leg_y_top - row_idx * 0.38
        for i, (color, label) in enumerate(row_items):
            lx = start_x + i * spacing
            patch = FancyBboxPatch((lx, row_y), 0.38, 0.18, boxstyle="round,pad=0.03",
                                    facecolor=color, edgecolor="#90A4AE", linewidth=0.4, zorder=2)
            ax.add_patch(patch)
            ax.text(lx + 0.50, row_y + 0.09, label, ha="left", va="center",
                    fontsize=5.2, fontfamily="monospace", color="#546E7A", zorder=5)

    path = os.path.join(OUT_DIR, "sfr_overview_master.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    print(f"  Saved: {path}  ({os.path.getsize(path)/1024:.0f} KB)")
    return path


# ═════════════════════════════════════════════════════════
#  Chart 2: Memory Organization Overview
# ═════════════════════════════════════════════════════════

def draw_memory_overview():
    fig, ax = plt.subplots(figsize=(16, 11))
    ax.set_xlim(0, 16)
    ax.set_ylim(0, 14)
    ax.set_aspect("equal")
    ax.axis("off")

    # ── Title (compact) ──
    ax.text(8, 13.7, "Standard 8051 Memory Organization  —  4 Address Spaces",
            ha="center", fontsize=13, fontweight="bold", fontfamily="monospace", color="#1A237E")
    # subtitle moved to bottom to avoid overlapping column titles

    COLS = {
        "rom_vec":   "#FF8A80", "rom_user":  "#FFAB91", "rom_ext":   "#FFCCBC",
        "ram_reg":   "#A5D6A7", "ram_bit":   "#CE93D8", "ram_user":  "#90CAF9",
        "sfr":       "#80CBC4", "sfr_idata": "#B2DFDB", "xram":      "#FFF176",
    }

    def block(ax, x, y, w, h, label, color, detail="", fs=8.5):
        rect = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.06",
                               facecolor=color, edgecolor="#455A64", linewidth=1.0, alpha=0.85)
        ax.add_patch(rect)
        ax.text(x + w/2, y + h/2, label, ha="center", va="center",
                fontsize=fs, fontfamily="monospace", color="#263238", fontweight="bold")
        if detail:
            ax.text(x + w/2, y + h/2 - 0.5, detail, ha="center", va="top",
                    fontsize=5.5, fontfamily="monospace", color="#455A64", linespacing=1.15)

    # ═══ Col 1: Program Memory ═══
    cx, cw = 0.35, 3.2
    ax.text(cx + cw/2, 13.05, "Program Memory\n(ROM / Flash)", ha="center",
            fontsize=9, fontfamily="monospace", color="#1A237E", fontweight="bold")

    block(ax, cx, 10.3, cw, 2.4,
          "On-Chip ROM  4 KB\n0x0000 – 0x0FFF", COLS["rom_vec"],
          "EA = 1  (internal)")
    # IVT overlay — taller box, two-line text, no overflow
    rect = FancyBboxPatch((cx+0.15, 11.95), cw-0.3, 0.62, boxstyle="round,pad=0.03",
                           facecolor="#FF5252", edgecolor="#B71C1C", linewidth=0.7, alpha=0.75, zorder=3)
    ax.add_patch(rect)
    ax.text(cx + cw/2, 12.26, "IVT  0x0000 – 0x0032\nReset + 5 Interrupts",
            ha="center", fontsize=5.2, fontfamily="monospace", color="white",
            fontweight="bold", zorder=5, linespacing=1.2)

    block(ax, cx, 5.8, cw, 4.2,
          "External ROM\nup to 64 KB\n0x0000 – 0xFFFF", COLS["rom_ext"],
          "EA=0 (all external)\nPSEN read strobe\nMOVC @A+DPTR / @A+PC")

    ax.text(cx + cw/2, 5.2, "8051: 4KB on-chip\n8052: 8KB on-chip\nPC starts at 0x0000",
            ha="center", fontsize=6.5, fontfamily="monospace", color="#90A4AE", fontstyle="italic")

    # ═══ Col 2: Internal RAM Low 128 ═══
    cx2 = cx + cw + 0.45
    ax.text(cx2 + cw/2, 13.05, "Internal RAM\nLow 128 Bytes\n(0x00 – 0x7F)", ha="center",
            fontsize=9, fontfamily="monospace", color="#1A237E", fontweight="bold")

    for i, (label, y) in enumerate([
        ("Bank 0  R0–R7\n0x00–0x07", 10.6),
        ("Bank 1  R0–R7\n0x08–0x0F", 9.85),
        ("Bank 2  R0–R7\n0x10–0x17", 9.1),
        ("Bank 3  R0–R7\n0x18–0x1F", 8.35),
    ]):
        block(ax, cx2, y, cw, 0.65, label, COLS["ram_reg"], "", 6.5)

    block(ax, cx2, 6.35, cw, 1.8,
          "Bit-Addressable\n16B x 8 = 128 bits\n0x20 – 0x2F", COLS["ram_bit"],
          "Bit addr 0x00–0x7F\nSETB / CLR / JB / JNB")

    block(ax, cx2, 4.0, cw, 2.15,
          "General Purpose\nScratchpad + Stack\n0x30 – 0x7F  (80 B)", COLS["ram_user"],
          "data variables\nSP reset = 0x07\n-> move to 0x60+")

    ax.text(cx2 + cw/2, 3.45, "Direct (MOV A, addr)\nIndirect (@R0, @R1)",
            ha="center", fontsize=6.5, fontfamily="monospace", color="#90A4AE", fontstyle="italic")

    # ═══ Col 3: Upper 128 / SFR ═══
    cx3 = cx2 + cw + 0.45
    ax.text(cx3 + cw/2, 13.05, "Upper 128 Bytes\n(0x80 – 0xFF)", ha="center",
            fontsize=9, fontfamily="monospace", color="#1A237E", fontweight="bold")

    block(ax, cx3, 9.4, cw, 3.35,
          "SFR Space\n(Direct Addressing)\n0x80 – 0xFF", COLS["sfr"],
          "P0,SP,DPL,DPH,PCON\nTCON,TMOD,TL0/1,TH0/1\nP1,SCON,SBUF,P2\nIE,P3,IP,PSW,ACC,B\n+T2CON,RCAP2,TL2,TH2")

    block(ax, cx3, 6.5, cw, 2.0,
          "Indirect RAM\n(8052 / Enhanced)\n0x80 – 0xFF", COLS["sfr_idata"],
          "MOV @R0/@R1, A\n128 extra bytes\nNot in 8051")

    ax.text(cx3 + cw/2, 5.9, "Same addr 0x80–0xFF:\n Direct -> SFR\n Indirect -> RAM (8052)",
            ha="center", fontsize=6.5, fontfamily="monospace", color="#90A4AE", fontstyle="italic")

    # ═══ Col 4: External Data RAM ═══
    cx4 = cx3 + cw + 0.45
    ax.text(cx4 + cw/2, 13.05, "External Data\nMemory (XRAM)", ha="center",
            fontsize=9, fontfamily="monospace", color="#1A237E", fontweight="bold")

    block(ax, cx4, 7.2, cw, 5.5,
          "External RAM\n64 KB\n0x0000 – 0xFFFF", COLS["xram"],
          "MOVX @DPTR, A\nMOVX A, @DPTR\n\nMOVX @R0/@R1, A\n(low 256B pdata)\n\nRD(P3.7)/WR(P3.6)\nP0+P2 addr/data bus\nALE latches low addr")

    # ── Inline legend (bottom-right, inside chart area) ──
    lx0, ly0 = 8.5, 2.6
    items = [
        (COLS["rom_vec"], "ROM/Flash"), (COLS["ram_reg"], "Reg Banks"),
        (COLS["ram_bit"], "Bit-Addr"),  (COLS["ram_user"], "Scratchpad"),
        (COLS["sfr"], "SFR"),           (COLS["xram"], "XRAM"),
    ]
    for i, (color, label) in enumerate(items):
        col = i % 3
        row = i // 3
        lx = lx0 + col * 2.6
        ly = ly0 - row * 0.55
        patch = FancyBboxPatch((lx, ly), 0.35, 0.22, boxstyle="round,pad=0.03",
                                facecolor=color, edgecolor="#455A64", linewidth=0.4)
        ax.add_patch(patch)
        ax.text(lx + 0.45, ly + 0.11, label, ha="left", va="center",
                fontsize=6, fontfamily="monospace", color="#37474F")

    # Harvard note at bottom
    ax.text(8, 1.55,
            "Harvard Architecture: separate Program Memory (ROM) and Data Memory (RAM) address spaces",
            ha="center", fontsize=7, fontfamily="monospace", color="#78909C", fontstyle="italic")

    # Keil C51 bar (inline, bottom)
    ax.text(8, 0.95,
            "Keil C51:  data (DIR low128)  |  idata (IND all256)  |  pdata (MOVX @R0 p0)  |  xdata (MOVX @DPTR)  |  code (MOVC ROM)",
            ha="center", fontsize=7.5, fontfamily="monospace", color="#1565C0", fontweight="bold",
            bbox=dict(boxstyle="round,pad=0.3", facecolor="#E3F2FD", edgecolor="#90CAF9"))

    path = os.path.join(OUT_DIR, "memory_overview.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    print(f"  Saved: {path}  ({os.path.getsize(path)/1024:.0f} KB)")
    return path


if __name__ == "__main__":
    print("Generating overview charts...")
    print("[1/2] SFR Register Master Overview...")
    draw_overview()
    print("[2/2] Memory Organization Overview...")
    draw_memory_overview()
    print("Done!")
