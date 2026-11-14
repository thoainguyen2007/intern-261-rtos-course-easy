#!/usr/bin/env python3
"""Week 3 Ex3 UART load test. Requires: pip install pyserial"""
import argparse
import time
import serial


def read_reply(ser, timeout_s=1.5):
    deadline = time.monotonic() + timeout_s
    data = bytearray()
    while time.monotonic() < deadline:
        data.extend(ser.read(ser.in_waiting or 1))
        if b'\n' in data:
            break
    return data.decode('utf-8', errors='replace').strip()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--port', default='COM7', help='Serial port, e.g. COM7')
    ap.add_argument('--count', type=int, default=1000)
    ap.add_argument('--baud', type=int, default=57600)
    ap.add_argument('--gap-ms', type=float, default=0, help='Optional gap after each command')
    args = ap.parse_args()
    if args.count <= 0 or args.gap_ms < 0:
        ap.error('count must be positive; gap-ms must not be negative')

    with serial.Serial(args.port, args.baud, timeout=0.1, write_timeout=5) as ser:
        time.sleep(0.3)
        ser.reset_input_buffer()
        ser.write(b'Z\r')
        ser.flush()
        ready = read_reply(ser, 1.0)
        if ready:
            print('STM32:', ready)
        else:
            print('No READY received (TX path or firmware may not support it).')
        t0 = time.monotonic()
        total_bytes = 0
        for i in range(args.count):
            command = f'P10#{i}\r'.encode('ascii')
            total_bytes += ser.write(command)
            if args.gap_ms:
                time.sleep(args.gap_ms / 1000.0)
        ser.flush()
        elapsed = time.monotonic() - t0
        # Allow the parser to drain before querying counts.
        time.sleep(0.5)
        ser.write(b'R\r')
        ser.flush()
        report = read_reply(ser, 2.0)

    print(f'PC sent: {args.count} commands, {total_bytes} bytes')
    print(f'Elapsed write+flush: {elapsed:.3f} s')
    print(f'Observed throughput: {total_bytes / elapsed:.1f} bytes/s')
    print('STM32 report:', report if report else '(no UART response)')
    if not report:
        print('WARNING: sent count does not prove received count or zero byte loss.')


if __name__ == '__main__':
    main()
