import i3

def onFocusChange(event, data, subscription):
	if event['change'] == 'focus': print(event['current']['num'])

onFocusChange = i3.Subscription(onFocusChange, 'workspace')
