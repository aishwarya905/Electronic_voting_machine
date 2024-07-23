import time
import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522

# Set up RFID reader
reader = SimpleMFRC522()

# Initialize GPIO pins
switch1 = 29
switch2 = 31
switch3 = 33
buzzer = 35

GPIO.setmode(GPIO.BOARD)
GPIO.setup(switch1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(switch2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(switch3, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(buzzer, GPIO.OUT)

# Initialize vote counts
bjp_votes = 0
congress_votes = 0
ncp_votes = 0
already_voted_tag = None

def button_pressed(pin):
    global bjp_votes, congress_votes, ncp_votes, already_voted_tag

    if pin == switch1:
        party = "BJP"
        bjp_votes += 1
    elif pin == switch2:
        party = "Congress"
        congress_votes += 1
    elif pin == switch3:
        party = "NCP"
        ncp_votes += 1

    print("Vote cast for", party)

    # Turn on buzzer to indicate vote cast
    GPIO.output(buzzer, GPIO.HIGH)
    time.sleep(0.5)
    GPIO.output(buzzer, GPIO.LOW)

    time.sleep(1)

try:
    GPIO.add_event_detect(switch1, GPIO.FALLING, callback=button_pressed, bouncetime=200)
    GPIO.add_event_detect(switch2, GPIO.FALLING, callback=button_pressed, bouncetime=200)
    GPIO.add_event_detect(switch3, GPIO.FALLING, callback=button_pressed, bouncetime=200)

    print("Scan your RFID tag to cast your vote.")

    while True:
        id, tag = reader.read()

        if id != already_voted_tag:
            already_voted_tag = id
            print("RFID tag ID:", id)
        else:
            print("You have already voted with this RFID tag. Please use a different tag.")

except KeyboardInterrupt:
    print("\nVoting process terminated.")

finally:
    # Print the final vote counts
    print("\nFinal vote counts:")
    print("BJP:", bjp_votes)
    print("Congress:", congress_votes)
    print("NCP:", ncp_votes)

    # Clean up GPIO
    GPIO.cleanup()