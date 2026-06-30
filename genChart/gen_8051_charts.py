#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_8051_charts.py — Standard 8051 MCU memory cell & register bit charts

Tight layout, z-order fixed: text (z=5) always on top of patches (z=1-3).
"""

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch
import numpy as np
import os

OUT_DIR = "charts"
os.makedirs(OUT_DIR, exist_ok=True)

plt.rcParams.update({
    "font.family": "monospace",
    "font.size": 9,
    "figure.dpi": 150,
    "savefig.dpi": 150,
    "savefig.bbox": "tight",
    "savefig.pad_inches": 0.08,
})

COLS = {
    "rom_vec":     "#FF6B6B",
    "rom_user":    "#4ECDC4",
    "ram_bank0":   "#A8E6CF",
    "ram_bank1":   "#DCEDC1",
    "ram_bank2":   "#FFD3B6",
    "ram_bank3":   "#FFAAA5",
    "ram_bit":     "#D4A5FF",
    "ram_user":    "#C8E6FF",
    "ram_stack":   "#FFE082",
    "ram_sfr":     "#B0BEC5",
    "reg_bit1":    "#81C784",
    "reg_bit0":    "#ECEFF1",
    "reg_label":   "#37474F",
    "border":      "#455A64",
    "grid_line":   "#CFD8DC",
}

# ═══════════════════════════════════════════════════════════
#  1. Internal RAM Low 128 Bytes
# ═══════════════════════════════════════════════════════════

def draw_ram_low128():
    fig, ax = plt.subplots(figsize=(16, 10))
    ax.set_xlim(-0.5, 13.5)
    ax.set_ylim(-1.0, 16.8)
    ax.set_aspect("equal")
    ax.axis("off")

    ax.text(4, 16.45, "Standard 8051 Internal RAM  Low 128 Bytes (0x00–0x7F)",
            ha="center", va="center", fontsize=14, fontweight="bold",
            fontfamily="monospace", color="#263238", zorder=10)

    regions = [
        ("Bank 0  R0–R7\n0x00 – 0x07",  0x00, 0x07, COLS["ram_bank0"]),
        ("Bank 1  R0–R7\n0x08 – 0x0F",  0x08, 0x0F, COLS["ram_bank1"]),
        ("Bank 2  R0–R7\n0x10 – 0x17",  0x10, 0x17, COLS["ram_bank2"]),
        ("Bank 3  R0–R7\n0x18 – 0x1F",  0x18, 0x1F, COLS["ram_bank3"]),
        ("Bit-Addressable\n16 B x 8 = 128 bits\n0x20 – 0x2F",  0x20, 0x2F, COLS["ram_bit"]),
        ("General Purpose\nScratchpad + Stack\n0x30 – 0x7F",   0x30, 0x7F, COLS["ram_user"]),
    ]

    # Quick lookup: addr -> color
    addr_to_color = {}
    for _label, start, end, c in regions:
        for a in range(start, end + 1):
            addr_to_color[a] = c

    # Cells: patch zorder=2, text zorder=5
    for addr in range(0x00, 0x80):
        row = addr // 8
        col = 7 - (addr % 8)
        x, y = col, 15 - row
        color = addr_to_color.get(addr, COLS["ram_user"])

        rect = FancyBboxPatch((x, y), 0.92, 0.92,
                              boxstyle="round,pad=0.04",
                              facecolor=color, edgecolor=COLS["border"],
                              linewidth=0.6, zorder=2)
        ax.add_patch(rect)
        ax.text(x + 0.46, y + 0.46, f"{addr:02X}H", ha="center", va="center",
                fontsize=5.5, fontfamily="monospace", color="#263238",
                fontweight="bold", zorder=5)

    # Column labels (bit position)
    for col in range(8):
        ax.text(col + 0.46, 16.2, f"bit {7-col}", ha="center", va="center",
                fontsize=7, fontfamily="monospace", color=COLS["reg_label"], zorder=10)

    # Row labels (left side)
    for r in range(16):
        base = r * 8
        ax.text(-0.25, 15 - r + 0.46, f"{base:02X}H", ha="right", va="center",
                fontsize=6.5, fontfamily="monospace", color=COLS["reg_label"],
                fontweight="bold", zorder=10)

    # Region labels on the right side — aligned with each region's vertical center
    label_x = 8.7
    for label_text, start, end, color in regions:
        start_row = start // 8
        end_row = end // 8
        y_center = 15 - (start_row + end_row) / 2   # data y of region midpoint

        # Small color swatch
        swatch = FancyBboxPatch((label_x, y_center - 0.15), 0.35, 0.30,
                                 boxstyle="round,pad=0.03",
                                 facecolor=color, edgecolor=COLS["border"],
                                 linewidth=0.6, zorder=3)
        ax.add_patch(swatch)
        # Label text next to swatch
        ax.text(label_x + 0.55, y_center, label_text, ha="left", va="center",
                fontsize=6.5, fontfamily="monospace", color="#37474F",
                fontweight="bold", zorder=5, linespacing=1.25)

    # Divider line between grid and labels
    ax.plot([8.35, 8.35], [0, 16], color="#CFD8DC", linewidth=0.8, zorder=1)

    # SP note at bottom
    ax.text(6.5, -0.5, "SP default = 0x07  ->  First PUSH writes to 0x08 (Bank 1 R0)  ->  Move SP to 0x60+ in main()",
            ha="center", fontsize=7, fontfamily="monospace", color="#D32F2F",
            fontstyle="italic", zorder=10,
            bbox=dict(boxstyle="round", facecolor="#FFEBEE", alpha=0.8, edgecolor="#EF9A9A"))

    path = os.path.join(OUT_DIR, "ram_low128.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


# ═══════════════════════════════════════════════════════════
#  2. ROM Interrupt Vector Table
# ═══════════════════════════════════════════════════════════

def draw_rom_ivt():
    fig, ax = plt.subplots(figsize=(13, 11))
    ax.set_xlim(-0.5, 12.5)
    ax.set_ylim(-4.5, 18)
    ax.set_aspect("equal")
    ax.axis("off")

    ax.text(6, 17.5, "Standard 8051 ROM: Interrupt Vector Table (0x0000–0x0032)",
            ha="center", fontsize=13, fontweight="bold", fontfamily="monospace",
            color="#263238", zorder=10)

    vectors = [
        ("0x0000", "Reset Vector\nLJMP MAIN",   "#FF5252", "Power-on / RST pin"),
        ("0x0003", "INT0\nExternal Int 0",       "#FF8A80", "P3.2  (IT0 edge/level)"),
        ("0x000B", "Timer 0\nOverflow",          "#FF80AB", "TF0 on FFFF->0000"),
        ("0x0013", "INT1\nExternal Int 1",       "#EA80FC", "P3.3  (IT1 edge/level)"),
        ("0x001B", "Timer 1\nOverflow",          "#B388FF", "TF1 on FFFF->0000"),
        ("0x0023", "UART\nSerial Port",          "#8C9EFF", "TI=1 or RI=1"),
        ("0x002B", "Timer 2\n(8052 only)",       "#80D8FF", "TF2 / EXF2 flag"),
    ]

    CELL_X = 3.5    # cells start at x=3.5, labels right-aligned at x=3.2
    ROW_SPACING = 2.35
    LABEL_X = 3.2    # right-align labels so they never cross into cells

    for i, (base_addr, label, color, desc) in enumerate(vectors):
        y_base = 15.5 - i * ROW_SPACING

        # Vector cells (8 bytes each)
        for byte_off in range(8):
            x = CELL_X + (7 - byte_off)
            y = y_base - 0.7
            rect = FancyBboxPatch((x, y), 0.88, 0.88,
                                  boxstyle="round,pad=0.04",
                                  facecolor=color, edgecolor=COLS["border"],
                                  linewidth=0.5, alpha=0.85, zorder=2)
            ax.add_patch(rect)
            by_addr = int(base_addr, 16) + byte_off
            ax.text(x + 0.44, y + 0.44, f"{by_addr:04X}H", ha="center", va="center",
                    fontsize=4.2, fontfamily="monospace", color="white",
                    fontweight="bold", zorder=5)

        # "LJMP (3B)" marker BELOW the first 3 cells — never covers addresses
        ljmp_x = CELL_X + 6 + 0.44   # center of byte_off=1 cell
        ljmp_y = y_base - 0.7 - 0.18  # just below the cell row
        ax.text(ljmp_x, ljmp_y, "LJMP\n(3B)", ha="center", va="top",
                fontsize=5.5, fontfamily="monospace", color="#E53935", fontweight="bold",
                zorder=5)

        # Label — right-aligned at LABEL_X, text extends LEFT, never touches cells
        ax.text(LABEL_X, y_base - 0.26, label, ha="right", va="center",
                fontsize=7.5, fontfamily="monospace", color=COLS["reg_label"],
                fontweight="bold", linespacing=1.2, zorder=10)

        # Description — right-aligned, below label
        ax.text(LABEL_X, y_base - 0.7, desc, ha="right", va="center",
                fontsize=6, fontfamily="monospace", color="#546E7A",
                fontstyle="italic", zorder=10)

    # User code area — representative 3 rows (24 cells) below Timer 2
    last_y = 15.5 - 6 * ROW_SPACING       # y_base of Timer 2 row
    user_start_y = last_y - 0.7 - 0.8     # gap below last row's cells

    USER_CODE_ROWS = 3
    for byte_off in range(USER_CODE_ROWS * 8):
        x = CELL_X + (7 - (byte_off % 8))
        y = user_start_y - (byte_off // 8) * 1.0
        rect = FancyBboxPatch((x, y), 0.88, 0.88,
                              boxstyle="round,pad=0.04",
                              facecolor=COLS["rom_user"], edgecolor=COLS["border"],
                              linewidth=0.5, alpha=0.85, zorder=2)
        ax.add_patch(rect)

    text_y = user_start_y - USER_CODE_ROWS - 0.5
    ax.text(CELL_X + 4, text_y,
            "0x0033  ->  User Program Area  (approx 4 KB on-chip ROM)",
            ha="center", fontsize=9, fontfamily="monospace", color="#00695C",
            fontweight="bold", zorder=10)

    ax.text(CELL_X + 4, text_y - 0.9,
            "Each vector slot = 8 bytes.  Short ISR fits inline;  otherwise place LJMP to handler.",
            ha="center", fontsize=7, fontfamily="monospace", color="#546E7A",
            fontstyle="italic", zorder=10)

    path = os.path.join(OUT_DIR, "rom_ivt.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


# ═══════════════════════════════════════════════════════════
#  3. Bit-Addressable Area (20H–2FH)
# ═══════════════════════════════════════════════════════════

def draw_bit_addressable():
    fig, ax = plt.subplots(figsize=(16, 7))
    ax.set_xlim(0, 8.5)
    ax.set_ylim(0, 16.8)
    ax.set_aspect("equal")
    ax.axis("off")

    ax.text(4, 16.45, "Standard 8051 Bit-Addressable Area  0x20–0x2F  (16 bytes x 8 = 128 addressable bits)",
            ha="center", fontsize=12, fontweight="bold", fontfamily="monospace",
            color="#263238", zorder=10)

    for byte_off in range(16):
        byte_addr = 0x20 + byte_off
        row = 15 - byte_off
        for bit in range(8):
            col = 7 - bit
            bit_addr = byte_off * 8 + bit
            x, y = col, row
            alpha_val = 0.5 + 0.5 * (bit_addr / 127.0)
            r = 0.83 * (0.5 + 0.5 * alpha_val)
            g = 0.65 * (0.5 + 0.5 * alpha_val)
            b = 1.0 * (0.5 + 0.5 * alpha_val)

            rect = FancyBboxPatch((x, y), 0.92, 0.92,
                                  boxstyle="round,pad=0.04",
                                  facecolor=(r, g, b), edgecolor="#7B1FA2",
                                  linewidth=0.5, zorder=2)
            ax.add_patch(rect)
            ax.text(x + 0.46, y + 0.46, f"{bit_addr:02X}H", ha="center", va="center",
                    fontsize=6.5, fontfamily="monospace", color="#311B92",
                    fontweight="bold", zorder=5)

        ax.text(-0.2, row + 0.46, f"{byte_addr:02X}H", ha="right", va="center",
                fontsize=7.5, fontfamily="monospace", color=COLS["reg_label"],
                fontweight="bold", zorder=10)

    for bit in range(8):
        ax.text(7 - bit + 0.46, 16.35, f"bit{bit}", ha="center", va="center",
                fontsize=7.5, fontfamily="monospace", color="#7B1FA2",
                fontweight="bold", zorder=10)

    ax.text(4, -0.5, "Bit Address = N x 8 + b    (N = byte index 0..15,  b = bit position 0..7)\n"
            "Use:  uchar bdata flags;  sbit LED = flags^3;",
            ha="center", fontsize=8, fontfamily="monospace", color="#6A1B9A",
            fontstyle="italic", zorder=10)

    path = os.path.join(OUT_DIR, "bit_addressable.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


# ═══════════════════════════════════════════════════════════
#  4. SFR Bit Diagrams (IE, IP, TCON, TMOD, SCON)
# ═══════════════════════════════════════════════════════════

def draw_sfr_bits(sfr_name, sfr_addr, bits, title, filename):
    n = len(bits)
    fig, ax = plt.subplots(figsize=(14, 5.5))
    ax.set_xlim(0, n * 1.5 + 0.5)
    ax.set_ylim(0, 6.5)
    ax.set_aspect("equal")
    ax.axis("off")

    ax.text((n * 1.5 + 0.5) / 2, 6.2, title, ha="center", fontsize=13,
            fontweight="bold", fontfamily="monospace", color="#263238", zorder=10)

    for i in range(n):
        x = i * 1.5 + 0.75
        ax.text(x, 5.8, f"bit {n-1-i}", ha="center", va="center",
                fontsize=7.5, fontfamily="monospace", color="#78909C", zorder=10)

    for i, bit in enumerate(bits):
        x_center = i * 1.5 + 0.75

        if bit.get("is_reserved"):
            rect = FancyBboxPatch((x_center - 0.55, 2.8), 1.1, 2.2,
                                  boxstyle="round,pad=0.08",
                                  facecolor="#ECEFF1", edgecolor="#B0BEC5",
                                  linewidth=1, zorder=2)
            ax.add_patch(rect)
            ax.text(x_center, 3.9, "(reserved)", ha="center", va="center",
                    fontsize=7, fontfamily="monospace", color="#90A4AE", zorder=5)
        else:
            # Bit=1 (top half)
            rect1 = FancyBboxPatch((x_center - 0.55, 3.8), 1.1, 1.2,
                                   boxstyle="round,pad=0.08",
                                   facecolor=COLS["reg_bit1"], edgecolor="#388E3C",
                                   linewidth=0.8, zorder=2)
            ax.add_patch(rect1)
            ax.text(x_center, 4.4, bit.get("on_label", "1"), ha="center", va="center",
                    fontsize=7, fontfamily="monospace", color="#1B5E20",
                    fontweight="bold", zorder=5)

            # Bit=0 (bottom half)
            rect0 = FancyBboxPatch((x_center - 0.55, 2.8), 1.1, 1.0,
                                   boxstyle="round,pad=0.08",
                                   facecolor=COLS["reg_bit0"], edgecolor="#90A4AE",
                                   linewidth=0.8, zorder=2)
            ax.add_patch(rect0)
            ax.text(x_center, 3.3, bit.get("off_label", "0"), ha="center", va="center",
                    fontsize=6.5, fontfamily="monospace", color="#546E7A", zorder=5)

        # Bit name (top)
        ax.text(x_center, 5.35, bit["name"], ha="center", va="center",
                fontsize=10, fontfamily="monospace", color=COLS["reg_label"],
                fontweight="bold", zorder=10)

        # Group label (bottom)
        if "group" in bit:
            ax.text(x_center, 2.3, bit["group"], ha="center", va="center",
                    fontsize=7, fontfamily="monospace", color="#5D4037",
                    fontweight="bold", zorder=5,
                    bbox=dict(boxstyle="round,pad=0.2", facecolor="#FFF3E0",
                              edgecolor="#FFCC80"))

        if not bit.get("is_reserved"):
            ax.text(x_center, 2.65, "RST=0", ha="center", va="center",
                    fontsize=5.5, fontfamily="monospace", color="#90A4AE", zorder=5)

    if sfr_addr:
        ax.text((n * 1.5 + 0.5) / 2, 0.5,
                f"SFR Address: {sfr_addr}    "
                f"{'Bit-Addressable' if any(b.get('name') for b in bits) else 'Not Bit-Addressable'}",
                ha="center", fontsize=8, fontfamily="monospace", color="#546E7A",
                fontstyle="italic", zorder=10)

    path = os.path.join(OUT_DIR, filename)
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


def gen_all_sfr_charts():
    ie_bits = [
        {"name": "EA",  "on_label": "Enable All\nInterrupts",  "off_label": "Disable ALL\ninterrupts"},
        {"name": "—",   "is_reserved": True},
        {"name": "ET2", "on_label": "Enable\nTimer 2",         "off_label": "Disable\nTimer 2"},
        {"name": "ES",  "on_label": "Enable\nSerial (UART)",   "off_label": "Disable\nSerial"},
        {"name": "ET1", "on_label": "Enable\nTimer 1",         "off_label": "Disable\nTimer 1"},
        {"name": "EX1", "on_label": "Enable\nINT1 (P3.3)",     "off_label": "Disable\nINT1"},
        {"name": "ET0", "on_label": "Enable\nTimer 0",         "off_label": "Disable\nTimer 0"},
        {"name": "EX0", "on_label": "Enable\nINT0 (P3.2)",     "off_label": "Disable\nINT0"},
    ]
    draw_sfr_bits("IE", "0xA8", ie_bits,
                  "IE — Interrupt Enable Register  (0xA8, Bit-Addressable)",
                  "ie_register.png")

    ip_bits = [
        {"name": "—",   "is_reserved": True},
        {"name": "—",   "is_reserved": True},
        {"name": "PT2", "on_label": "Timer 2\nHigh Priority",  "off_label": "Timer 2\nLow Priority"},
        {"name": "PS",  "on_label": "Serial\nHigh Priority",   "off_label": "Serial\nLow Priority"},
        {"name": "PT1", "on_label": "Timer 1\nHigh Priority",  "off_label": "Timer 1\nLow Priority"},
        {"name": "PX1", "on_label": "INT1\nHigh Priority",     "off_label": "INT1\nLow Priority"},
        {"name": "PT0", "on_label": "Timer 0\nHigh Priority",  "off_label": "Timer 0\nLow Priority"},
        {"name": "PX0", "on_label": "INT0\nHigh Priority",     "off_label": "INT0\nLow Priority"},
    ]
    draw_sfr_bits("IP", "0xB8", ip_bits,
                  "IP — Interrupt Priority Register  (0xB8, Bit-Addressable)",
                  "ip_register.png")

    tcon_bits = [
        {"name": "TF1", "on_label": "T1 Overflow\n(HW sets)",           "off_label": "No overflow\n(cleared by ISR)"},
        {"name": "TR1", "on_label": "T1 RUN\n(Start)",                  "off_label": "T1 STOP"},
        {"name": "TF0", "on_label": "T0 Overflow\n(HW sets)",           "off_label": "No overflow\n(cleared by ISR)"},
        {"name": "TR0", "on_label": "T0 RUN\n(Start)",                  "off_label": "T0 STOP"},
        {"name": "IE1", "on_label": "INT1 Edge\nDetected",              "off_label": "No edge\n(cleared by ISR)"},
        {"name": "IT1", "on_label": "INT1: Falling\nEdge Trigger",      "off_label": "INT1: Low\nLevel Trigger"},
        {"name": "IE0", "on_label": "INT0 Edge\nDetected",              "off_label": "No edge\n(cleared by ISR)"},
        {"name": "IT0", "on_label": "INT0: Falling\nEdge Trigger",      "off_label": "INT0: Low\nLevel Trigger"},
    ]
    draw_sfr_bits("TCON", "0x88", tcon_bits,
                  "TCON — Timer/Counter Control Register  (0x88, Bit-Addressable)",
                  "tcon_register.png")

    tmod_bits = [
        {"name": "GATE","on_label": "T1 runs only if\nINT1 pin = 1", "off_label": "T1 runs when\nTR1 = 1",   "group": "Timer 1\n(high nibble)"},
        {"name": "C/T", "on_label": "Counter\n(count T1 pin)",       "off_label": "Timer\n(count cycles)",    "group": "Timer 1"},
        {"name": "M1",  "on_label": "Mode bit 1\n(see mode table)",  "off_label": "Mode bit 1\n(see table)",  "group": "Timer 1"},
        {"name": "M0",  "on_label": "Mode bit 0\n(see mode table)",  "off_label": "Mode bit 0\n(see table)",  "group": "Timer 1"},
        {"name": "GATE","on_label": "T0 runs only if\nINT0 pin = 1", "off_label": "T0 runs when\nTR0 = 1",   "group": "Timer 0\n(low nibble)"},
        {"name": "C/T", "on_label": "Counter\n(count T0 pin)",       "off_label": "Timer\n(count cycles)",    "group": "Timer 0"},
        {"name": "M1",  "on_label": "Mode bit 1\n(see mode table)",  "off_label": "Mode bit 1\n(see table)",  "group": "Timer 0"},
        {"name": "M0",  "on_label": "Mode bit 0\n(see mode table)",  "off_label": "Mode bit 0\n(see table)",  "group": "Timer 0"},
    ]
    draw_sfr_bits("TMOD", "0x89", tmod_bits,
                  "TMOD — Timer Mode Register  (0x89, NOT Bit-Addressable)",
                  "tmod_register.png")

    scon_bits = [
        {"name": "SM0", "on_label": "Mode bit 0\n(see mode table)", "off_label": "Mode bit 0\n(see table)"},
        {"name": "SM1", "on_label": "Mode bit 1\n(see mode table)", "off_label": "Mode bit 1\n(see table)"},
        {"name": "SM2", "on_label": "Multi-MCU\n(addr detect)",     "off_label": "Normal\nUART"},
        {"name": "REN", "on_label": "Receive\nEnabled",             "off_label": "Receive\nDisabled"},
        {"name": "TB8", "on_label": "9th Tx bit\n= 1",             "off_label": "9th Tx bit\n= 0"},
        {"name": "RB8", "on_label": "9th Rx bit\n= 1",             "off_label": "9th Rx bit\n= 0"},
        {"name": "TI",  "on_label": "Tx complete\n(SW must clear!)","off_label": "Tx idle"},
        {"name": "RI",  "on_label": "Rx complete\n(SW must clear!)","off_label": "Rx idle"},
    ]
    draw_sfr_bits("SCON", "0x98", scon_bits,
                  "SCON — Serial Port Control Register  (0x98, Bit-Addressable)",
                  "scon_register.png")


# ═══════════════════════════════════════════════════════════
#  5. SFR Map (80H–FFH)
# ═══════════════════════════════════════════════════════════

def draw_sfr_map():
    fig, ax = plt.subplots(figsize=(14, 14))
    ax.set_xlim(0, 16)
    ax.set_ylim(0, 8.5)
    ax.set_aspect("equal")
    ax.axis("off")

    ax.text(8, 8.2, "Standard 8051 SFR Map  0x80–0xFF  (128 address slots)",
            ha="center", fontsize=13, fontweight="bold", fontfamily="monospace",
            color="#263238", zorder=10)

    sfrs = {
        0x80: ("P0", True),   0x81: ("SP", False),   0x82: ("DPL", False),
        0x83: ("DPH", False), 0x87: ("PCON", False), 0x88: ("TCON", True),
        0x89: ("TMOD", False),0x8A: ("TL0", False),   0x8B: ("TL1", False),
        0x8C: ("TH0", False), 0x8D: ("TH1", False),
        0x90: ("P1", True),
        0x98: ("SCON", True), 0x99: ("SBUF", False),
        0xA0: ("P2", True),
        0xA8: ("IE", True),
        0xB0: ("P3", True),
        0xB8: ("IP", True),
        0xD0: ("PSW", True),
        0xE0: ("ACC", True),
        0xF0: ("B", True),
    }

    for addr in range(0x80, 0x100):
        col = (addr - 0x80) % 16
        row = 7 - (addr - 0x80) // 16
        x, y = col, row

        if addr in sfrs:
            name, bit_addr = sfrs[addr]
            color = "#81C784" if bit_addr else "#64B5F6"
            edge = "#2E7D32" if bit_addr else "#1565C0"
        else:
            name, color, edge = "", "#ECEFF1", "#B0BEC5"

        rect = FancyBboxPatch((x, y), 0.92, 0.92,
                              boxstyle="round,pad=0.04",
                              facecolor=color, edgecolor=edge, linewidth=0.8,
                              zorder=2)
        ax.add_patch(rect)

        if name:
            ax.text(x + 0.46, y + 0.58, name, ha="center", va="center",
                    fontsize=7.5, fontfamily="monospace", color="white",
                    fontweight="bold", zorder=5)
            ax.text(x + 0.46, y + 0.22, f"{addr:02X}H", ha="center", va="center",
                    fontsize=4.5, fontfamily="monospace", color="white", zorder=5)
        else:
            ax.text(x + 0.46, y + 0.46, f"{addr:02X}H", ha="center", va="center",
                    fontsize=5.5, fontfamily="monospace", color="#90A4AE", zorder=5)

    for c in range(16):
        ax.text(c + 0.46, 8.0, f"+{c:X}", ha="center", va="center",
                fontsize=6, fontfamily="monospace", color="#78909C", zorder=10)
    for r in range(8):
        base = 0x80 + r * 16
        ax.text(-0.3, 7 - r + 0.46, f"{base:02X}H", ha="right", va="center",
                fontsize=6.5, fontfamily="monospace", color="#546E7A",
                fontweight="bold", zorder=10)

    lp = [mpatches.Patch(color="#81C784", label="Bit-Addressable SFR"),
          mpatches.Patch(color="#64B5F6", label="Non-Bit-Addressable SFR"),
          mpatches.Patch(color="#ECEFF1", label="Reserved / Unused")]
    ax.legend(handles=lp, loc="lower left", bbox_to_anchor=(0.02, -0.02),
              ncol=3, fontsize=7, framealpha=0.9)

    ax.text(8, -0.7, "Only SFRs at addresses ending in 0x0 or 0x8 are bit-addressable in standard 8051",
            ha="center", fontsize=7.5, fontfamily="monospace", color="#546E7A",
            fontstyle="italic", zorder=10)

    path = os.path.join(OUT_DIR, "sfr_map.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


# ═══════════════════════════════════════════════════════════
#  6. Timer Modes
# ═══════════════════════════════════════════════════════════

def draw_timer_modes():
    fig, axes = plt.subplots(1, 4, figsize=(16, 5))
    fig.suptitle("Standard 8051 Timer/Counter — 4 Operating Modes",
                 fontsize=13, fontweight="bold", fontfamily="monospace", y=0.98)

    modes = [
        ("Mode 0: 13-bit Timer", "THx (8 bit)", "TLx (low 5 bit)", "#FFCDD2",
         "Compatible with 8048\nRarely used today"),
        ("Mode 1: 16-bit Timer", "THx (8 bit)", "TLx (8 bit)", "#C8E6C9",
         "Most common\nPrecise timing\nManual reload needed"),
        ("Mode 2: 8-bit Auto-Reload", "THx\n(Reload Value)", "TLx (8-bit Counter)\nOverflow->Reload", "#BBDEFB",
         "UART Baud Rate Gen.\nAuto-reload from THx\nNo software reload"),
        ("Mode 3: Split Timer (T0 only)", "TH0 (8-bit)\nIndependent Timer 1", "TL0 (8-bit)\nIndependent Timer 2", "#FFF9C4",
         "T0 splits into two\n8-bit timers\nT1 stops if used"),
    ]

    for ax_i, (title, hi, lo, color, note) in zip(axes, modes):
        ax_i.set_xlim(0, 4)
        ax_i.set_ylim(0, 6)
        ax_i.set_aspect("equal")
        ax_i.axis("off")
        ax_i.set_title(title, fontsize=10, fontfamily="monospace", fontweight="bold",
                       color="#37474F")

        r1 = FancyBboxPatch((0.5, 2.5), 3, 1.5, boxstyle="round,pad=0.1",
                            facecolor=color, edgecolor="#455A64", linewidth=1.5, zorder=2)
        ax_i.add_patch(r1)
        ax_i.text(2, 3.25, hi, ha="center", va="center", fontsize=9,
                  fontfamily="monospace", color="#263238", fontweight="bold", zorder=5)

        r2 = FancyBboxPatch((0.5, 0.8), 3, 1.5, boxstyle="round,pad=0.1",
                            facecolor=color, edgecolor="#455A64", linewidth=1.5,
                            alpha=0.7, zorder=2)
        ax_i.add_patch(r2)
        ax_i.text(2, 1.55, lo, ha="center", va="center", fontsize=8,
                  fontfamily="monospace", color="#37474F", zorder=5)

        ax_i.text(2, 5.3, note, ha="center", va="center", fontsize=7.5,
                  fontfamily="monospace", color="#546E7A", fontstyle="italic", zorder=10,
                  bbox=dict(boxstyle="round,pad=0.3", facecolor="#FAFAFA", edgecolor="#E0E0E0"))

    fig.tight_layout()
    path = os.path.join(OUT_DIR, "timer_modes.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


# ═══════════════════════════════════════════════════════════
#  7. Interrupt Priority
# ═══════════════════════════════════════════════════════════

def draw_interrupt_priority():
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 9)
    ax.set_aspect("equal")
    ax.axis("off")

    ax.text(5, 8.5, "Standard 8051 Interrupt — Natural Priority & Vector Address",
            ha="center", fontsize=13, fontweight="bold", fontfamily="monospace",
            color="#263238", zorder=10)

    intrs = [
        ("1 (Highest)", "INT0", "0x0003", "P3.2 — External Interrupt 0", "#FF5252"),
        ("2", "Timer 0", "0x000B", "TF0 — Timer 0 Overflow", "#FF7043"),
        ("3", "INT1", "0x0013", "P3.3 — External Interrupt 1", "#FFA726"),
        ("4", "Timer 1", "0x001B", "TF1 — Timer 1 Overflow", "#66BB6A"),
        ("5", "UART", "0x0023", "TI / RI — Serial Port", "#42A5F5"),
        ("6 (Lowest)", "Timer 2", "0x002B", "TF2 / EXF2 (8052 only)", "#AB47BC"),
    ]

    for i, (pri, name, vector, desc, color) in enumerate(intrs):
        y = 7.5 - i * 1.2

        # Priority badge
        badge = FancyBboxPatch((0.2, y - 0.35), 1.1, 0.7,
                               boxstyle="round,pad=0.05",
                               facecolor=color, edgecolor="#37474F", linewidth=1.2, zorder=3)
        ax.add_patch(badge)
        ax.text(0.75, y, pri, ha="center", va="center", fontsize=8,
                fontfamily="monospace", color="white", fontweight="bold", zorder=5)

        # Arrow
        ax.annotate("", xy=(1.5, y), xytext=(2.5, y),
                    arrowprops=dict(arrowstyle="->", color="#546E7A", lw=1.5), zorder=2)

        # Name block
        box = FancyBboxPatch((2.6, y - 0.35), 2.2, 0.7,
                             boxstyle="round,pad=0.08",
                             facecolor=color, edgecolor="#37474F", linewidth=0.8,
                             alpha=0.3, zorder=2)
        ax.add_patch(box)
        ax.text(3.7, y, name, ha="center", va="center", fontsize=10,
                fontfamily="monospace", color="#263238", fontweight="bold", zorder=5)

        # Vector address
        ax.text(5.3, y, f"-> {vector}", ha="left", va="center", fontsize=9,
                fontfamily="monospace", color="#D32F2F", fontweight="bold", zorder=5)

        # Description
        ax.text(7.5, y, desc, ha="left", va="center", fontsize=8,
                fontfamily="monospace", color="#546E7A", zorder=5)

    ax.text(5, 0.3, "Polling order: Hardware checks in this sequence every machine cycle.\n"
            "High-priority (IP=1) ISR can preempt low-priority (IP=0) ISR — 2-level nesting only.",
            ha="center", fontsize=7.5, fontfamily="monospace", color="#78909C",
            fontstyle="italic", zorder=10)

    path = os.path.join(OUT_DIR, "interrupt_priority.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


# ═══════════════════════════════════════════════════════════
#  8. Memory Type Keywords
# ═══════════════════════════════════════════════════════════

def draw_memory_types():
    fig, axes = plt.subplots(1, 5, figsize=(18, 6))
    fig.suptitle("Keil C51 Memory Type Keywords — Address Space Visualization",
                 fontsize=13, fontweight="bold", fontfamily="monospace", y=0.98)

    types = [
        ("data",  "Direct Addressing\n0x00–0x7F (128B)",       "#A5D6A7",
         "Fastest access\nInternal RAM low 128B\nMOV A, addr"),
        ("idata", "Indirect Addressing\n0x00–0xFF (256B)",     "#81C784",
         "@R0 / @R1\nInternal RAM all 256B\n(8052 for 0x80+)"),
        ("pdata", "Paged External\n0x00–0xFF (256B)",          "#64B5F6",
         "MOVX @R0 / @R1\nExternal RAM page 0\nSlower than data"),
        ("xdata", "Full External\n0x0000–0xFFFF (64KB)",       "#42A5F5",
         "MOVX @DPTR\nExternal RAM full range\nSlowest access"),
        ("code",  "Program Memory\n0x0000–0xFFFF (64KB)",       "#FF8A65",
         "MOVC @A+DPTR\nFlash / ROM\nRead-only constants\nTables, fonts, strings"),
    ]

    for ax_i, (name, desc, color, detail) in zip(axes, types):
        ax_i.set_xlim(0, 4)
        ax_i.set_ylim(0, 5)
        ax_i.set_aspect("equal")
        ax_i.axis("off")
        ax_i.set_title(name, fontsize=12, fontfamily="monospace", fontweight="bold",
                       color="#37474F", pad=8)

        box = FancyBboxPatch((0.3, 1.5), 3.4, 2.5, boxstyle="round,pad=0.12",
                             facecolor=color, edgecolor="#37474F", linewidth=1.5,
                             alpha=0.5, zorder=2)
        ax_i.add_patch(box)
        ax_i.text(2, 2.75, desc, ha="center", va="center", fontsize=8,
                  fontfamily="monospace", color="#263238", fontweight="bold", zorder=5)

        ax_i.text(2, 0.8, detail, ha="center", va="center", fontsize=7,
                  fontfamily="monospace", color="#455A64", linespacing=1.4, zorder=5)

    fig.tight_layout()
    path = os.path.join(OUT_DIR, "memory_types.png")
    fig.savefig(path, facecolor="white")
    plt.close(fig)
    return path


# ═══════════════════════════════════════════════════════════
#  MAIN
# ═══════════════════════════════════════════════════════════

def main():
    files = []
    print("Generating charts...")
    print("  [1] RAM low 128 bytes...")
    files.append(draw_ram_low128())
    print("  [2] ROM interrupt vector table...")
    files.append(draw_rom_ivt())
    print("  [3] Bit-addressable area (20H-2FH)...")
    files.append(draw_bit_addressable())
    print("  [4] SFR map...")
    files.append(draw_sfr_map())
    print("  [5] SFR bit diagrams (IE, IP, TCON, TMOD, SCON)...")
    gen_all_sfr_charts()
    for n in ["ie_register.png","ip_register.png","tcon_register.png",
              "tmod_register.png","scon_register.png"]:
        files.append(os.path.join(OUT_DIR, n))
    print("  [6] Timer modes...")
    files.append(draw_timer_modes())
    print("  [7] Interrupt priority...")
    files.append(draw_interrupt_priority())
    print("  [8] Memory type keywords...")
    files.append(draw_memory_types())
    print(f"\nDone! {len(files)} charts saved to {OUT_DIR}/")
    for f in files:
        print(f"  {os.path.basename(f):30s}  {os.path.getsize(f)/1024:6.1f} KB")


if __name__ == "__main__":
    main()
