# ICMP Echo Request and Reply in C

## Description

This program is used to send ICMP echo requests and receive ICMP echo replies. It aims to demonstrate the packet-level communication using the ICMP protocol in a Linux environment. The program is aligned with the following RFCs:

- [RFC 792](https://tools.ietf.org/html/rfc792) "Internet Control Message Protocol"
- [RFC 791](https://tools.ietf.org/html/rfc791) "Internet Header Format"
- [RFC 760](https://tools.ietf.org/html/rfc760) "Internet Protocol"

For ICMP parameter types, refer to the [IANA assignments](https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml).

## Pre-requisites

- This program must be run as root.
- The program is currently built for Linux systems.

## Packet Capture

You can capture the ICMP packets sent or received by this program using packet capturing tools like Wireshark, tshark or tcpdump:

```bash
tcpdump -X -s0 -i eth0 -p icmp
```

## Socket Creation Limitations

- **Linux**: Non-root users can create up to 1024 sockets per process. For root users, this limit is typically around 65,000.

  To check or modify these limits, use the `ulimit -n [new limit]` command. For permanent changes, edit the `/etc/security/limits.conf` file.

- **RHEL Specific**: Similar to other Linux distributions, the maximum number of open files (including sockets) is often set to 1024 per process for non-root users.

## TODOs

- **OS Support**: Add support for OSX and Windows.
- **Dynamic Gateway**: Dynamically retrieve the MAC address of the gateway.
- **ICMP Error Messages**: Add support for ICMP error messages.
- **ICMP Redirect**: Add support for ICMP redirect messages.
- **ICMP Timestamp**: Add support for ICMP timestamp messages.
- **ICMP Address Mask**: Add support for ICMP address mask messages.
- **ICMP Router Discovery**: Add support for ICMP router discovery messages.
- **ICMP Traceroute**: Add support for ICMP traceroute messages.

## License

This project is open-source, under the MIT [LICENSE](LICENSE).

## Disclaimer

Please use this program responsibly and ethically. Unauthorized packet sending or sniffing may be illegal in some jurisdictions. The author of this program is not responsible for any misuse of this program.