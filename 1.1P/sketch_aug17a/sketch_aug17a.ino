const int PORCH_LED = 5;
const int HALLWAY_LED = 7;
const int BUTTON = 3;

void setup()
{
    pinMode(PORCH_LED, OUTPUT);
    pinMode(HALLWAY_LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    digitalWrite(PORCH_LED, LOW);
    digitalWrite(HALLWAY_LED, LOW);
}

void loop()
{
    if (digitalRead(BUTTON) == LOW)
    {
        // Turn both lights on
        digitalWrite(PORCH_LED, HIGH);
        digitalWrite(HALLWAY_LED, HIGH);

        // Porch stays on for 30 seconds
        delay(30000);
        digitalWrite(PORCH_LED, LOW);

        // Hallway stays on for another 30 seconds
        delay(60000);
        digitalWrite(HALLWAY_LED, LOW);

        // Wait for button to be released
        while (digitalRead(BUTTON) == LOW)
        {
            delay(10);
        }
    }
}