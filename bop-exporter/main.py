from flask import Flask, Response
import ctypes

app = Flask(__name__)
icmp_ping_lib = ctypes.CDLL('./icmp_ping.so')

@app.route("/metrics")
def metrics():
    latency = icmp_ping_lib.icmp_ping()
    metrics = f'icmp_latency{{target="{TARGET}"}} {latency}\n'
    return Response(metrics, content_type="text/plain; version=0.0.4; charset=utf-8")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)
