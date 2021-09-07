for i in 'cat string_addr.txt'; do
    if snmpwalk -v2 -c $i  &> /dev/null
    then echo "$i" >> snmp-ok   # SNMP OK
    then echo "$i" >> snmp-nok  # SNMP NOK
    fi
done
