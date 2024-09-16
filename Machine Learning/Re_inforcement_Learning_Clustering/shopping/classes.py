

class evidence_class:

    def __init__(self,administrative,administrative_duration,information, informational_duration, product_related,productRelated_duration,bounce_rates,exit_rates,page_values,special_day, month,operating_systems,browser,region,traffic_type,visitor_type, weekend):
        
        month = month.lower()
        visitor = visitor_type.lower()
        months = {
            'jan': int(0),
            'feb': int(1),
            'mar': int(2),
            'apr': int(3),
            'may': int(4),
            'june': int(5),
            'jul': int(6),
            'aug': int(7),
            'sep': int(8),
            'oct': int(9),
            'nov': int(10),
            'dec': int(11)
        }
        visitors = {
            'new_visitor':int(0),
            'returning_visitor':int(1)
        }
        self.administrative = int(administrative)
        self.information = int(information)
        self.product_related = int(product_related)
        
        self.month = int(months.get(month,200))
        #print('month: ',month) if self.month==200 else print(end='')
        #print(self.month,end=' ')
        self.operating_systems = int(operating_systems)
        self.browser = int(browser)
        self.region = int(region)
        self.traffic_type = int(traffic_type)
        self.visitor_type = 1 if visitor == 'returning_visitor' else 0 
        #print('visitor_type: ', visitor)
        #print('Special visitor_type: ',visitor) if self.visitor_type==500 else print(end='')
        #print(self.visitor_type,end=' ')
        self.weekend = int(weekend== 'TRUE')
        # print(self.weekend)
        self.administrative_duration = float(administrative_duration)
        self.informational_duration = float(informational_duration) 
        self.productRelated_duration = float(productRelated_duration)
        self.bounce_rates = float(bounce_rates)
        self.exit_rates = float(exit_rates)
        self.page_values = float(page_values)
        self.special_day = float(special_day)
    
    def list_evidence(self):
        evidence = [
            self.administrative, 
            self.administrative_duration,       
            self.information,    
            self.informational_duration ,   
            self.product_related,    
            self.productRelated_duration, 
            self.bounce_rates, 
            self.exit_rates,
            self.page_values,
            self.special_day,
            self.month,     
            self.operating_systems,    
            self.browser,   
            self.region,  
            self.traffic_type, 
            self.visitor_type,
            self.weekend              
        ]
        return  evidence


"""
    Let's keep this just in case
"""
class evidence_float:

    def __init__(self,administrative_duration, informational_duration,productRelated_duration,bounce_rates,exit_rates,page_values,special_day):
        self.administrative_duration = float(administrative_duration)
        self.informational_duration = float(informational_duration) 
        self.productRelated_duration = float(productRelated_duration)
        self.bounce_rates = float(bounce_rates)
        self.exit_rates = float(exit_rates)
        self.page_values = float(page_values)
        self.special_day = float(special_day)
