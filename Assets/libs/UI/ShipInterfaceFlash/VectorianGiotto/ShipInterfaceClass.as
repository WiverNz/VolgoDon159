class ShipInterfaceClass
{
	static var s_shipInterface:ShipInterfaceClass;
	static var s_isTesting:Boolean = true;
	static var s_rudderValue:Number = 0;
	static var s_speedValue:Number = 0;

    public static function main(mc:MovieClip):Void
    {
		trace("Ship interface class initialization");
        s_shipInterface = new ShipInterfaceClass();
    }

	public function onKeyDown(event:Key):Void
	{
		switch(Key.getCode())
		{
		case 37: // LEFT ARROW ←
		case 65: // A
			_root.SetRudderValue(++s_rudderValue);
			break;
		case 39: // RIGHT ARROW →
		case 68: // D
			_root.SetRudderValue(--s_rudderValue);
			break;
		case 40: // DOWN ARROW ↓
		case 87: // W
			_root.SetSpeedValue(--s_speedValue);
			break;
		case 38: // UP ARROW ↑
		case 83: // W
			_root.SetSpeedValue(++s_speedValue);
			break;
		}
	}
	
	public function ShipInterfaceClass() 
	{
		Key.addListener(this);
		_root.SetRudderValue(0);
		_root.SetSpeedValue(0);
	}
}