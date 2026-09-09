#include "Input.h"

namespace gll
{
	class Stock
	{
		Instance instance;

	public:

		void createStock();
		void deleteStock();
	};

	Stock* stock();
}