import { Router } from 'express';
import { MakeUpService } from './service';
import { AuthRequest } from '../../types';

const router = Router();

// Get business events for the authenticated user
router.get('/business-events', async (req: AuthRequest, res) => {
  try {
    const userId = req.user?.id;
    if (!userId) {
      return res.status(401).json({
        success: false,
        message: 'User not authenticated'
      });
    }

    const service = new MakeUpService();
    const events = await service.getBusinessEvents(userId);
    
    res.json({
      success: true,
      data: {
        events
      }
    });
  } catch (error) {
    console.error('Error fetching business events:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to fetch business events'
    });
  }
});

// Get a specific business event
router.get('/business-events/:eventId', async (req: AuthRequest, res) => {
  try {
    const userId = req.user?.id;
    if (!userId) {
      return res.status(401).json({
        success: false,
        message: 'User not authenticated'
      });
    }

    const { eventId } = req.params;
    const service = new MakeUpService();
    
    const event = await service.getBusinessEvent(userId, eventId);
    
    if (!event) {
      return res.status(404).json({
        success: false,
        message: 'Business event not found'
      });
    }
    
    res.json({
      success: true,
      data: {
        event
      }
    });
  } catch (error) {
    console.error('Error fetching business event:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to fetch business event'
    });
  }
});

// Add a single business event
router.post('/business-events/event', async (req: AuthRequest, res) => {
  try {
    const userId = req.user?.id;
    if (!userId) {
      return res.status(401).json({
        success: false,
        message: 'User not authenticated'
      });
    }

    const event = req.body;
    const service = new MakeUpService();
    
    await service.addBusinessEvent(userId, event);
    
    res.json({
      success: true,
      message: 'Business event added successfully'
    });
  } catch (error) {
    console.error('Error adding business event:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to add business event'
    });
  }
});

// Update a specific business event
router.put('/business-events/:eventId', async (req: AuthRequest, res) => {
  try {
    const userId = req.user?.id;
    if (!userId) {
      return res.status(401).json({
        success: false,
        message: 'User not authenticated'
      });
    }

    const { eventId } = req.params;
    const updatedEvent = req.body;
    const service = new MakeUpService();
    
    await service.updateBusinessEvent(userId, eventId, updatedEvent);
    
    res.json({
      success: true,
      message: 'Business event updated successfully'
    });
  } catch (error) {
    console.error('Error updating business event:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to update business event'
    });
  }
});

// Delete a specific business event
router.delete('/business-events/:eventId', async (req: AuthRequest, res) => {
  try {
    const userId = req.user?.id;
    if (!userId) {
      return res.status(401).json({
        success: false,
        message: 'User not authenticated'
      });
    }

    const { eventId } = req.params;
    const service = new MakeUpService();
    
    await service.deleteBusinessEvent(userId, eventId);
    
    res.json({
      success: true,
      message: 'Business event deleted successfully'
    });
  } catch (error) {
    console.error('Error deleting business event:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to delete business event'
    });
  }
});

export default router;
