from flask import Flask, Response
import threading
import subprocess
import schedule

app = Flask(__name__)

def run_c_program():
    result = subprocess.run(['./icmp.out'], stdout=subprocess.PIPE)
    output = result.stdout.decode('utf-8')
    with open('icmp_c_output.txt', 'w') as f:
        f.write(output)

@app.route('/metrics')
def metrics():
    with open('icmp_c_output.txt', 'r') as f:
        content = f.read()
    return Response(content, content_type='text/plain; charset=utf-8')

def run_schedule():
    while True:
        schedule.run_pending()

if __name__ == "__main__":
    run_c_program()  # Run once at the beginning
    schedule.every(5).minutes.do(run_c_program)

    t = threading.Thread(target=run_schedule)
    t.start()

    app.run(host='0.0.0.0', port=9999)