import { Router } from 'express';
import { CalendarService } from './service';
import { AuthRequest, CalendarItemRequest } from '../../types';

const router = Router();
const calendarService = new CalendarService();

// GET /api/calendar/items - Get all calendar items (user's private + all public)
router.get('/items', async (req: AuthRequest, res) => {
  try {
    if (!req.user) {
      return res.status(401).json({ error: 'Unauthorized' });
    }

    const items = await calendarService.getCalendarItems(req.user.id);
    res.json({ 
      success: true,
      data: { items }
    });
  } catch (error) {
    console.error('Error getting calendar items:', error);
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to get calendar items'
    });
  }
});

// POST /api/calendar/items - Create a new calendar item
router.post('/items', async (req: AuthRequest, res) => {
  try {
    if (!req.user) {
      return res.status(401).json({ error: 'Unauthorized' });
    }

    const itemData: CalendarItemRequest = req.body;
    
    // Validate required fields
    if (!itemData.text || !itemData.dateTime || !itemData.endTime || !itemData.color) {
      return res.status(400).json({ 
        error: 'Bad request',
        message: 'Missing required fields: text, dateTime, endTime, color'
      });
    }

    // Generate ID if not provided
    const id = itemData.id || Date.now().toString();
    
    await calendarService.addCalendarItem(req.user.id, {
      id,
      text: itemData.text,
      dateTime: itemData.dateTime,
      endTime: itemData.endTime,
      isPrivate: itemData.isPrivate || false,
      color: itemData.color,
    });

    res.status(201).json({ 
      success: true,
      message: 'Calendar item created successfully',
      data: { id }
    });
  } catch (error) {
    console.error('Error creating calendar item:', error);
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to create calendar item'
    });
  }
});

// PUT /api/calendar/items/:id - Update a calendar item
router.put('/items/:id', async (req: AuthRequest, res) => {
  try {
    if (!req.user) {
      return res.status(401).json({ error: 'Unauthorized' });
    }

    const itemId = req.params.id;
    const itemData: CalendarItemRequest = req.body;
    
    // Validate required fields
    if (!itemData.text || !itemData.dateTime || !itemData.endTime || !itemData.color) {
      return res.status(400).json({ 
        error: 'Bad request',
        message: 'Missing required fields: text, dateTime, endTime, color'
      });
    }

    await calendarService.updateCalendarItem(req.user.id, itemId, {
      id: itemId,
      text: itemData.text,
      dateTime: itemData.dateTime,
      endTime: itemData.endTime,
      isPrivate: itemData.isPrivate || false,
      color: itemData.color,
    });

    res.json({ 
      success: true,
      message: 'Calendar item updated successfully'
    });
  } catch (error) {
    console.error('Error updating calendar item:', error);
    if (error instanceof Error && error.message.includes('not found')) {
      res.status(404).json({ 
        error: 'Not found',
        message: 'Calendar item not found'
      });
    } else if (error instanceof Error && error.message.includes('Not authorized')) {
      res.status(403).json({ 
        error: 'Forbidden',
        message: 'Not authorized to update this calendar item'
      });
    } else {
      res.status(500).json({ 
        error: 'Internal server error',
        message: 'Failed to update calendar item'
      });
    }
  }
});

// DELETE /api/calendar/items/:id - Delete a calendar item
router.delete('/items/:id', async (req: AuthRequest, res) => {
  try {
    if (!req.user) {
      return res.status(401).json({ error: 'Unauthorized' });
    }

    const itemId = req.params.id;
    
    // Check if item exists and belongs to user
    const existingItem = await calendarService.getCalendarItem(req.user.id, itemId);
    if (!existingItem) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Calendar item not found'
      });
    }

    if (existingItem.createdBy !== req.user.id) {
      return res.status(403).json({ 
        error: 'Forbidden',
        message: 'Not authorized to delete this calendar item'
      });
    }

    await calendarService.deleteCalendarItem(req.user.id, itemId);

    res.json({ 
      success: true,
      message: 'Calendar item deleted successfully'
    });
  } catch (error) {
    console.error('Error deleting calendar item:', error);
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to delete calendar item'
    });
  }
});

// GET /api/calendar/items/:id - Get a specific calendar item
router.get('/items/:id', async (req: AuthRequest, res) => {
  try {
    if (!req.user) {
      return res.status(401).json({ error: 'Unauthorized' });
    }

    const itemId = req.params.id;
    const item = await calendarService.getCalendarItem(req.user.id, itemId);
    
    if (!item) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Calendar item not found'
      });
    }

    res.json({ 
      success: true,
      data: item
    });
  } catch (error) {
    console.error('Error getting calendar item:', error);
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to get calendar item'
    });
  }
});

export default router;
