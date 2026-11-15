# Whether the hardware interface talks to the StepIt controller, published on
# ~/status of the hardware's node, e.g. /motors/status. Latched: published when
# it changes and every second, so that a subscriber that stops receiving it
# knows that the node is gone.

builtin_interfaces/Time stamp

# True while the controller answers: from a successful handshake until a read
# or a write fails. ros2_control then moves the hardware to unconfigured, and
# it stays disconnected until ros2_control_node starts again.
bool connected

# The serial port of the controller, e.g. /dev/ttyACM0, or "fake" for the
# fake driver.
string device

# Why it is not connected, e.g. the error of the serial port; empty when it is.
string message
