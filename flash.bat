cd F:/
avrdude -c usbasp -p 8052 -C "F:/NANDU/Projects/Github/Mini Project/avrdude-at89s52.conf.txt" -B 250 -e -U flash:w:"NANDU\Projects\Github\Mini Project\Pill Dispencer\pill_dispenser3.0\Objects\pill.hex" 

pause