#include "Stock.h"

namespace gll
{
	Stock ss;
	Stock* stock()
	{
		return &ss;
	}

	void Stock::createStock()
	{
		instance.createInstance();
	}
	void Stock::deleteStock()
	{
		instance.deleteInstance();
	}
}