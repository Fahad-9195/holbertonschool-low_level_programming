            found_digit = 1;
            // Check for overflow before multiplying and adding
            if (sign == 1)
            {
                if (result > (INT_MAX - (s[i] - '0')) / 10)
                    return INT_MAX;
            }
            else
            {
                if (result > (-(INT_MIN + (s[i] - '0'))) / 10)
                    return INT_MIN;
            }
            result = result * 10 + (s[i] - '0');
        }
        else if (found_digit)
            break;
        i++;
    }
    return (sign * result);
}
