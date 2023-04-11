int pt1, pt2, pt3, pt4, pt5, tc1, tc2, tc3, lc1, lc2;

// assign values to variables

int values[] = {pt1, pt2, pt3, pt4, pt5, tc1, tc2, tc3, lc1, lc2};
int numValues = sizeof(values) / sizeof(values[0]);

// Print out numbers separated by comma to serial
for (int i = 0; i < numValues; i++)
{
    Serial.print(values[i]);

    if (i < numValues - 1)
    {
        Serial.print(",");
    }
}

Serial.println();