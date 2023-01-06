#!/bin/sh

key=""
email="oguzhan.ince@protonmail.com"
zone="cerebnismus.com"
zone_id=""
type=A
name="nm"
content=$(curl -s http://myexternalip.com/raw)
ttl=360 # means auto-TTL
proxied=true # means use the proxied IP address

# TEST
# curl -X GET "https://api.cloudflare.com/client/v4/zones/XXXXXX" \
#      -H "Content-Type:application/json" \
#      -H "X-Auth-Key:" \
#      -H "X-Auth-Email:oguzhan.ince@protonmail.com"

# GET RECORDS
# curl -X GET "https://api.cloudflare.com/client/v4/zones/XXXXXXX/dns_records?type=A&page=1&per_page=100&order=type&direction=desc&match=all" \
#      -H "Content-Type:application/json" \
#      -H "X-Auth-Key:" \
#      -H "X-Auth-Email:oguzhan.ince@protonmail.com"

# TO:DO = SET RECORD ID FROM $GET_RECORDS output
rec_id=""

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
    echo "Existing record value $existing_content is the same as provided content $content. Exiting."
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