# Hardware Schematic and Topology Guide

## 12V distribution

The pitboard should be powered from a clean 12V source with transient suppression at the input. A recommended pattern is:

- 12V input -> reverse-polarity protection -> TVS -> fuse -> switch -> distribution bus
- 12V bus -> 5V buck converter for display logic
- 12V bus -> 3.3V buck regulator for ESP32 and shift register logic
- Dedicated low-ESR bulk capacitance near the LED and display drivers

## Segment driver topology

The 18-digit wall should be grouped into three rows of six digits. Each row can be controlled with a synchronized driver chain:

- row strobe lines select active display bank
- shift register chain serializes segment data
- blanking / dimming controlled by PWM or latch pulse timing
- common row drive uses MOSFETs or dedicated transistor array

This enables bright outdoor readability while reducing peak current spikes.

## GPIO layout and signal integrity

- Use short, tightly routed SPI signals and a solid ground reference.
- Place decoupling capacitors at shift register VCC and display driver power pins.
- Route touch and display control lines away from high-current LED returns.
- For the external telemetry panel, isolate logic and panel backlight grounds when practical.

## Best practices

- Add pull-up / pull-down resistors at control lines where needed.
- Provide clear labeling for each display row and power rail in the PCB silkscreen.
- Reserve footprint for optional current measurement or future sensor expansion.
- Include test points for 12V, 5V, 3.3V, and key GPIO groups.
