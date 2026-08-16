#!/usr/bin/env python3
"""
SCPI interface conformance test over a serial port (UART/VCP).

Usage:
    python3 tools/test_scpi.py -p /dev/ttyACM0
    python3 tools/test_scpi.py -p COM5 -b 115200

Requires pyserial (pip install pyserial).
"""

import argparse
import sys
import unittest

import serial

PORT = None
BAUDRATE = 115200
TIMEOUT = 2.0


class ScpiLink:
    """Thin request/response wrapper around a pyserial connection."""

    def __init__(self, port, baudrate, timeout):
        self.ser = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
        # Flush any stale bytes (e.g. leftover from a previous session).
        self.ser.reset_input_buffer()

    def close(self):
        self.ser.close()

    def send(self, command):
        self.ser.write(command.encode("ascii") + b"\r\n")

    def read_line(self):
        line = self.ser.readline()
        if not line:
            raise TimeoutError(f"no response within {self.ser.timeout}s")
        return line.decode("ascii", errors="replace").strip()

    def query(self, command):
        self.send(command)
        return self.read_line()

    def drain_errors(self):
        """Pop SYSTem:ERRor? until the queue is empty, returning the list."""
        errors = []
        while True:
            resp = self.query("SYSTem:ERRor?")
            code = resp.split(",", 1)[0]
            if code == "0":
                break
            errors.append(resp)
            if len(errors) > 20:
                raise AssertionError("error queue not draining, got: " + repr(errors))
        return errors


class ScpiTestCase(unittest.TestCase):
    link: ScpiLink = None

    @classmethod
    def setUpClass(cls):
        cls.link = ScpiLink(PORT, BAUDRATE, TIMEOUT)

    @classmethod
    def tearDownClass(cls):
        cls.link.close()

    def setUp(self):
        # Start every test from a clean slate.
        self.link.send("*CLS")
        self.link.drain_errors()

    def test_idn_has_four_fields(self):
        idn = self.link.query("*IDN?")
        fields = idn.split(",")
        self.assertEqual(len(fields), 4, f"*IDN? reply malformed: {idn!r}")
        manufacturer, model, serial_no, fw = fields
        self.assertTrue(manufacturer)
        self.assertTrue(model)

    def test_opc_query_returns_one(self):
        resp = self.link.query("*OPC?")
        self.assertEqual(resp, "1")

    def test_tst_query_returns_zero(self):
        resp = self.link.query("*TST?")
        self.assertEqual(resp, "0")

    def test_cls_clears_error_queue(self):
        self.link.query("BOGUS:COMMAND")  # forces at least one queued error
        self.link.send("*CLS")
        resp = self.link.query("SYSTem:ERRor?")
        self.assertTrue(resp.startswith("0,"), f"expected empty queue, got {resp!r}")

    def test_unknown_command_reports_error(self):
        self.link.send("THIS:IS:NOT:A:COMMAND")
        resp = self.link.query("SYSTem:ERRor?")
        self.assertFalse(resp.startswith("0,"), "expected an error to be queued")

    def test_system_version_format(self):
        resp = self.link.query("SYSTem:VERSion?")
        # SCPI std version string, e.g. "1999.0"
        self.assertRegex(resp, r"^\d{4}\.\d+$")

    def test_output_state_roundtrip(self):
        self.link.query("OUTPut ON")
        self.assertEqual(self.link.query("OUTPut?"), "1")

        self.link.query("OUTPut OFF")
        self.assertEqual(self.link.query("OUTPut?"), "0")

    def test_rst_turns_output_off(self):
        self.link.query("OUTPut ON")
        self.assertEqual(self.link.query("OUTPut?"), "1")

        self.link.send("*RST")
        self.assertEqual(self.link.query("OUTPut?"), "0")

    def test_no_errors_left_over_after_suite(self):
        errors = self.link.drain_errors()
        self.assertEqual(errors, [])


def main():
    global PORT, BAUDRATE, TIMEOUT

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-p", "--port", required=True, help="serial port, e.g. /dev/ttyACM0 or COM5")
    parser.add_argument("-b", "--baudrate", type=int, default=BAUDRATE)
    parser.add_argument("-t", "--timeout", type=float, default=TIMEOUT)
    parser.add_argument("unittest_args", nargs="*", help="extra args forwarded to unittest")
    args = parser.parse_args()

    PORT = args.port
    BAUDRATE = args.baudrate
    TIMEOUT = args.timeout

    sys.argv[1:] = args.unittest_args
    unittest.main()


if __name__ == "__main__":
    main()
