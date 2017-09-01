import subprocess
from threading import Timer
import sys, os
from Queue import Queue

p = [0.25, 0, 5]
best_p = []
best_err = 10000;
dp = [0.0108900000000000002, 0.0, 0.12100000000000002]
it =0
step = 0
i = 0
def twiddle_step_0():
    global i
    print("Iteration {}, best error = {}".format(it, best_err))
    p[i] += dp[i]

def twiddle_step_1(err):
    global best_err,i, best_p
    if err < best_err:
        best_err = err
        best_p = get_output_message();
        dp[i] *= 1.1
        return True
    else:
        p[i] -= 2 * dp[i]
        return False

def twiddle_step_2(err):
    global best_err,i, best_p
    if err < best_err:
        if p[i]>0:
            best_err = err
            best_p = get_output_message();
            dp[i] *= 1.1
        else:
            p[i] += dp[i]
    else:
        p[i] += dp[i]
        dp[i] *= 0.9

def twiddle(err):
    global step, i, it, best_err
    if sum(dp) <0.08:
        print("optimized")
    print(best_err, err, step, p, best_p, dp)
    if dp[i] == 0:
        i = (i+1)%3
    if step ==0:
        twiddle_step_0()
        step = 1
    elif step ==1:
        if twiddle_step_1(err):
            step = 0
            i= (i+1)%3
            it+=1
        else:
            step = 2
    elif step ==2:
        twiddle_step_2(err)
        step = 0
        i=(i+1)%3
        it+=1
def main():
    path =  os.path.join(os.getcwd(),'build', 'pid')
    args = (path, '-as',str(p[0])+','+str(p[1])+','+str(p[2])+',', '-f', '500,1000')
    print(args)
    with open('test.log', 'w') as f:
        process = subprocess.Popen(args, stdout=subprocess.PIPE,  stdin=subprocess.PIPE, stderr=open('error.log', 'w'))
        for line in iter(process.stdout.readline, ''):
            f.write(line)
            sys.stdout.write(line)
            process_message(line, process)

def process_message(message, process):
    global best_err
    start, end = (message.rfind('$')+1,message.rfind('@'))
    clean_msg = message[start:end]
    if message.find('$') !=-1:
        if clean_msg == 'input':
            output_message = get_output_message()
            process.stdin.write(output_message)
        else:
            twiddle(float(clean_msg))

def get_output_message():
    return str(p[0]) + ' ' +  str(p[1]) + ' ' +  str(p[2]) + '\n'

if __name__ == '__main__':
    main()
