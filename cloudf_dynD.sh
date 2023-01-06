#!/bin/sh

# CloudFlare Dynamic DNS Client
# https://gist.github.com/chappy84/9606755
# Updates CloudFlare records with the current public IP address
#
# Takes the same basic arguments as A/CNAME updates in the CloudFlare API
# https://api.cloudflare.com/#dns-records-for-a-zone-list-dns-records
#
# Use with cron jobs etc. to update your dynamic DNS records automatically
# manually run: cloudflare_dyn_dns.sh -key 404613183ab3971a2118ae5bf03d63e032f9e -email oguzhan.ince@protonmail.com -zone nm.cerebnismus.com -name extra
#
# cronjob entry to run every 1 minutes: $ crontab -e and add the following line
# */1 * * * * /root/networkman-web/cloudflare_dyn_dns.sh -key 404613183ab3971a2118ae5bf03d63e032f9e -email test@example.com -zone example.com -name extra >> /path/to/cloudflare_dyn_dns.log
# will both set the type A DNS record for extra.example.com to the current public IP address for user test@example.com with the provided API key

key="beb7392e741f17cea755f5fc1c353c39086f3"
email="oguzhan.ince@protonmail.com"
zone="cerebnismus.com"
zone_id="5a7d2bbfeb4629c0d7e7a2cdd59e78cc"
type=A
name="nm"
content=$(curl -s http://myexternalip.com/raw)
ttl=60 # means auto-TTL
proxied=false # means use the proxied IP address

# TEST
# curl -X GET "https://api.cloudflare.com/client/v4/zones/5a7d2bbfeb4629c0d7e7a2cdd59e78cc" \
#      -H "Content-Type:application/json" \
#      -H "X-Auth-Key:beb7392e741f17cea755f5fc1c353c39086f3" \
#      -H "X-Auth-Email:oguzhan.ince@protonmail.com"

# GET RECORDS
# curl -X GET "https://api.cloudflare.com/client/v4/zones/5a7d2bbfeb4629c0d7e7a2cdd59e78cc/dns_records?type=A&page=1&per_page=100&order=type&direction=desc&match=all" \
#      -H "Content-Type:application/json" \
#      -H "X-Auth-Key:1234567876543f5fc12342356f3235" \
#      -H "X-Auth-Email:oguzhan.ince@protonmail.com"

# TO:DO = SET RECORD ID FROM $GET_RECORDS output
# c3460bc01dc4e8a756382c5ecc315394
rec_id="c3460bc01dc4e8a756382c5ecc315394"

if [ "$content" = "" ]
then
    content=`curl -s https://wtfismyip.com/text`
    if [ "$content" = "" ]
    then
        date
        echo "No IP address to set record value with."
        exit 1
    fi
fi

if [ "$name" = "" ]
then
    echo "You must provide the name of the record you wish to change."
    exit 1
fi

if [ "$zone" = "" ]
then
    echo "You must provide the domain you wish to change."
    exit 1
fi

if [ "$name" = "$zone" ]
then
    hostname="$name"
else
    hostname="$name.$zone"
fi

if [ "$key" = "" ]
then
    echo "You must provide your user API token."
    exit 1
fi

if [ "$email" = "" ]
then
    echo "You must provide your user email."
    exit 1
fi

command -v host > /dev/null 2>&1
if [ "$?" = "1" ]
then
    command -v nslookup > /dev/null 2>&1
    if [ "$?" = "1" ]
    then
        echo "Cannot find a way to check existing $type record for $hostname"
        exit 1
    fi
    existing_content=`nslookup -type=$type $hostname | awk -F 'Address: ' 'NR==6 { print $2 }'`
else
    existing_content=`host -t $type $hostname | sed -E 's/.+?\s+([^\s]+)$/\1/'`
fi

if [ "$content" != "$existing_content" ]
then

    # Update the DNS record
    update_response=`curl -s -X PUT "https://api.cloudflare.com/client/v4/zones/$zone_id/dns_records/$rec_id" \
                        -H "Content-Type:application/json" \
                        -H "X-Auth-Key:$key" \
                        -H "X-Auth-Email:$email" \
                        -d '{"type":"A","name":"'$hostname'","content":"'$content'","ttl":'$ttl',"proxied":'$proxied'}'`

    success_val=`echo $update_response | sed -E "s/.+\"success\":(true|false).+/\1/g"`
    if [ "$success_val" = "true" ]
    then
        echo "$(date +%d/%m/%Y-%T) - Record Updated Success - $existing_content -> $content" >> /var/log/cloudflare_dyn_dns.log
    else
        echo "$(date +%d/%m/%Y-%T) - Record Update Failed - $existing_content -> $content" >> /var/log/cloudflare_dyn_dns.log
    fi
# else
        # echo "$(date +%d/%m/%Y-%T) - Record Update Waiting - $existing_content -> $content" >> /var/log/cloudflare_dyn_dns.log
fi
