import { Router } from 'express';
import { DisciplineService } from './service';
import { AuthRequest } from '../../types';

const router = Router();
const disciplineService = new DisciplineService();

// Middleware to ensure only snake user can access discipline app
const snakeOnly = (req: AuthRequest, res: any, next: any) => {
  const userId = req.user?.id;
  if (userId !== 'snake') {
    return res.status(403).json({
      success: false,
      message: 'Access denied. Discipline app is only available for user snake.'
    });
  }
  next();
};

// Apply snake-only middleware to all discipline routes
router.use(snakeOnly);

// Get discipline data (coins, pools, next coupon price)
router.get('/data', async (req: AuthRequest, res) => {
  try {
    const service = new DisciplineService();
    const data = await service.getDisciplineData();
    
    res.json({
      success: true,
      data
    });
  } catch (error) {
    console.error('Error fetching discipline data:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to fetch discipline data'
    });
  }
});

// Add coins (specific color or random)
router.post('/coins/add', async (req: AuthRequest, res) => {
  try {
    const { color, amount } = req.body;
    
    if (!color || !amount || amount <= 0) {
      return res.status(400).json({
        success: false,
        message: 'Invalid request. Color and positive amount are required.'
      });
    }

    const service = new DisciplineService();
    const coins = await service.addCoins(color, amount);
    
    res.json({
      success: true,
      message: `Added ${amount} ${color} coin(s)`,
      data: {
        coins
      }
    });
  } catch (error) {
    console.error('Error adding coins:', error);
    res.status(500).json({
      success: false,
      message: error instanceof Error ? error.message : 'Failed to add coins'
    });
  }
});

// Manually buy a coupon
router.post('/coupons/buy', async (req: AuthRequest, res) => {
  try {
    const service = new DisciplineService();
          const result = await service.buyCoupon();
    
    if (!result.success) {
      return res.status(400).json({
        success: false,
        message: result.message
      });
    }
    
         res.json({
       success: true,
       message: result.message,
       data: {
         color: result.color,
         poolFilled: result.poolFilled
       }
     });
  } catch (error) {
    console.error('Error buying coupon:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to buy coupon'
    });
  }
});

// Get next coupon price
router.get('/coupons/price', async (req: AuthRequest, res) => {
  try {
    const service = new DisciplineService();
    const price = await service.getNextCouponPrice();
    
    res.json({
      success: true,
      data: {
        price
      }
    });
  } catch (error) {
    console.error('Error fetching coupon price:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to fetch coupon price'
    });
  }
});



// Get all pools
router.get('/pools', async (req: AuthRequest, res) => {
  try {
    const service = new DisciplineService();
    const pools = await service.getPools();
    
    res.json({
      success: true,
      data: {
        pools
      }
    });
  } catch (error) {
    console.error('Error fetching pools:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to fetch pools'
    });
  }
});

// Create a new pool
router.post('/pools', async (req: AuthRequest, res) => {
  try {
    const { name, maxAmount } = req.body;
    
    if (!name || !maxAmount || maxAmount <= 0) {
      return res.status(400).json({
        success: false,
        message: 'Invalid request. Name and positive maxAmount are required.'
      });
    }

    const service = new DisciplineService();
    const pool = await service.createPool(name, maxAmount);
    
    res.json({
      success: true,
      message: 'Pool created successfully',
      data: {
        pool
      }
    });
  } catch (error) {
    console.error('Error creating pool:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to create pool'
    });
  }
});

// Update a pool
router.put('/pools/:poolId', async (req: AuthRequest, res) => {
  try {
    const { poolId } = req.params;
    const updates = req.body;
    
    // Remove id from updates if present
    delete updates.id;
    
    if (updates.maxAmount !== undefined && updates.maxAmount <= 0) {
      return res.status(400).json({
        success: false,
        message: 'Max amount must be positive'
      });
    }

    const service = new DisciplineService();
    const pool = await service.updatePool(poolId, updates);
    
    res.json({
      success: true,
      message: 'Pool updated successfully',
      data: {
        pool
      }
    });
  } catch (error) {
    console.error('Error updating pool:', error);
    res.status(500).json({
      success: false,
      message: error instanceof Error ? error.message : 'Failed to update pool'
    });
  }
});

// Delete a pool
router.delete('/pools/:poolId', async (req: AuthRequest, res) => {
  try {
    const { poolId } = req.params;
    
    const service = new DisciplineService();
    await service.deletePool(poolId);
    
    res.json({
      success: true,
      message: 'Pool deleted successfully'
    });
  } catch (error) {
    console.error('Error deleting pool:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to delete pool'
    });
  }
});

// Work Burden routes
router.get('/workburden', async (req: AuthRequest, res) => {
  try {
    const workBurden = await disciplineService.getWorkBurden();
    res.json({
      success: true,
      data: { workBurden }
    });
  } catch (error) {
    console.error('Error getting work burden:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get work burden'
    });
  }
});

router.post('/workburden/decrease', async (req: AuthRequest, res) => {
  try {
    const { amount } = req.body;
    
    if (typeof amount !== 'number' || amount <= 0) {
      return res.status(400).json({
        success: false,
        message: 'Amount must be a positive number'
      });
    }

    const result = await disciplineService.decreaseWorkBurden(amount);
    res.json({
      success: true,
      message: `Work burden decreased by ${amount}`,
      data: result
    });
  } catch (error) {
    console.error('Error decreasing work burden:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to decrease work burden'
    });
  }
});


// Notebook routes
router.get('/notebook', async (req: AuthRequest, res) => {
  try {
    const notebook = await disciplineService.getNotebook();
    res.json({
      success: true,
      data: { notebook }
    });
  } catch (error) {
    console.error('Error getting notebook:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get notebook'
    });
  }
});

router.put('/notebook', async (req: AuthRequest, res) => {
  try {
    const { content } = req.body;
    
    if (typeof content !== 'string') {
      return res.status(400).json({
        success: false,
        message: 'Content must be a string'
      });
    }

    await disciplineService.updateNotebook(content);
    res.json({
      success: true,
      message: 'Notebook updated successfully'
    });
  } catch (error) {
    console.error('Error updating notebook:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to update notebook'
    });
  }
});

// Modifier routes
router.get('/modifiers', async (req: AuthRequest, res) => {
  try {
    const modifiers = await disciplineService.getModifiers();
    res.json({
      success: true,
      data: { modifiers }
    });
  } catch (error) {
    console.error('Error getting modifiers:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get modifiers'
    });
  }
});

router.post('/modifiers', async (req: AuthRequest, res) => {
  try {
    const { name, color, value } = req.body;
    
    if (!name || typeof name !== 'string' || !name.trim()) {
      return res.status(400).json({
        success: false,
        message: 'Name is required'
      });
    }
    
    if (!color || typeof color !== 'string') {
      return res.status(400).json({
        success: false,
        message: 'Color is required'
      });
    }
    
    if (typeof value !== 'number') {
      return res.status(400).json({
        success: false,
        message: 'Value must be a number'
      });
    }

    const modifier = await disciplineService.createModifier(name.trim(), color, value);
    res.json({
      success: true,
      message: 'Modifier created successfully',
      data: { modifier }
    });
  } catch (error) {
    console.error('Error creating modifier:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to create modifier'
    });
  }
});

router.put('/modifiers/:modifierId', async (req: AuthRequest, res) => {
  try {
    const { modifierId } = req.params;
    const { name, color, value } = req.body;
    
    const updates: any = {};
    if (name !== undefined) {
      if (typeof name !== 'string' || !name.trim()) {
        return res.status(400).json({
          success: false,
          message: 'Name must be a non-empty string'
        });
      }
      updates.name = name.trim();
    }
    
    if (color !== undefined) {
      if (typeof color !== 'string') {
        return res.status(400).json({
          success: false,
          message: 'Color must be a string'
        });
      }
      updates.color = color;
    }
    
    if (value !== undefined) {
      if (typeof value !== 'number') {
        return res.status(400).json({
          success: false,
          message: 'Value must be a number'
        });
      }
      updates.value = value;
    }

    const modifier = await disciplineService.updateModifier(modifierId, updates);
    res.json({
      success: true,
      message: 'Modifier updated successfully',
      data: { modifier }
    });
  } catch (error) {
    console.error('Error updating modifier:', error);
    if (error instanceof Error && error.message === 'Modifier not found') {
      res.status(404).json({
        success: false,
        message: 'Modifier not found'
      });
    } else {
      res.status(500).json({
        success: false,
        message: 'Failed to update modifier'
      });
    }
  }
});

router.delete('/modifiers/:modifierId', async (req: AuthRequest, res) => {
  try {
    const { modifierId } = req.params;
    await disciplineService.deleteModifier(modifierId);
    res.json({
      success: true,
      message: 'Modifier deleted successfully'
    });
  } catch (error) {
    console.error('Error deleting modifier:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to delete modifier'
    });
  }
});

export default router;
