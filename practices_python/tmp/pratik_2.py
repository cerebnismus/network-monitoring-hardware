#!/usr/bin/ python
# -*- coding: utf-8 -*-
# github.com/cerebnismus


def get_kg():
    
    try:
        kg = int(input("Enter your weight in kg: "))
    except ValueError:
        print("Please enter a number")
        get_kg()

    print("Your weight is: ", kg)
    
    print("Your weight in pounds is: ", kg * 2)
    
get_kg()


    driverinstance.execute_script('var element=document.getElementById("g-recaptcha-response"); element.style.display="";')   
     
    # driverinstance.execute_script("""document.getElementById("g-recaptcha-response").innerHTML = arguments[0]""", g_response)    
    # driverinstance.execute_script('var element=document.getElementById("g-recaptcha-response"); element.style.display="none";')

    # https://stackoverflow.com/questions/67773431/injecting-token-received-from-anticaptcha-for-recaptcha-v2-using-selenium-python
    driverinstance.execute_script('document.getElementById("g-recaptcha-response").innerHTML = "{}";'.format(g_response)) 
    time.sleep(1)

    # whatever the next step is. Could be clicking on a submit button
    # driverinstance.refresh()
    
    driverinstance.find_element(By.XPATH, '//*[@class="mat-focus-indicator btn mat-btn-lg btn-block btn-brand-orange mat-stroked-button mat-button-base ng-star-inserted"]').click()
    