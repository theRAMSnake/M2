import axios from 'axios';
import { logger } from './logger';

export interface WasteCollectionEvent {
  type: string;
  date: Date;
  description: string;
}

export class WasteCalendarFetcher {
  private readonly postalCode = '5627HC';
  private readonly houseNumber = '75';
  private readonly city = 'Eindhoven';

  // Dutch month names mapping
  private readonly dutchMonths: { [key: string]: number } = {
    'januari': 1, 'februari': 2, 'maart': 3, 'april': 4,
    'mei': 5, 'juni': 6, 'juli': 7, 'augustus': 8,
    'september': 9, 'oktober': 10, 'november': 11, 'december': 12
  };

  /**
   * Fetch waste collection calendar for the whole year starting from current date
   */
  async fetchWasteCalendar(): Promise<WasteCollectionEvent[]> {
    try {
      logger.info(`Fetching waste calendar for ${this.postalCode} ${this.houseNumber}, ${this.city}`);
      
      // Use the direct URL approach that works
      const calendarUrl = `https://www.mijnafvalwijzer.nl/nl/${this.postalCode}/${this.houseNumber}`;
      
      logger.info(`Accessing calendar URL: ${calendarUrl}`);
      
      const response = await axios.get(calendarUrl, {
        timeout: 10000,
        headers: {
          'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
          'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
          'Accept-Language': 'en-US,en;q=0.5',
          'Accept-Encoding': 'gzip, deflate',
          'Connection': 'keep-alive',
          'Upgrade-Insecure-Requests': '1'
        }
      });

      if (response.status !== 200) {
        logger.error('Failed to access calendar page');
        return [];
      }

      const html = response.data;
      logger.info(`Calendar page HTML length: ${html.length} characters`);
      
      // Extract all events from HTML
      const allEvents = this.extractFromHtml(html);
      
      logger.info(`Extracted ${allEvents.length} total waste collection events`);
      
      // Filter events to include the whole year starting from current date
      const filteredEvents = this.filterEventsForWholeYear(allEvents);
      
      logger.info(`Filtered to ${filteredEvents.length} events for the whole year`);
      
      return filteredEvents;

    } catch (error) {
      logger.error('Error fetching waste calendar:', error);
      return [];
    }
  }

  /**
   * Extract waste collection events from HTML using Dutch date format
   */
  private extractFromHtml(html: string): WasteCollectionEvent[] {
    const events: WasteCollectionEvent[] = [];
    
    logger.info('Extracting events from HTML...');
    
    // Find all Dutch date patterns: "woensdag 03 september" or "03 september"
    const dutchDatePattern = /(\w+)\s+(\d{1,2})\s+(\w+)/g;
    const dutchDates: Array<{date: Date, index: number, fullMatch: string}> = [];
    
    let match;
    while ((match = dutchDatePattern.exec(html)) !== null) {
      const [, dayOfWeek, day, month] = match;
      const dayNum = parseInt(day);
      
      // Check if month is in our Dutch months dictionary
      const monthLower = month.toLowerCase();
      if (this.dutchMonths[monthLower]) {
        const monthNum = this.dutchMonths[monthLower];
        
        // Assume current year (2025) for future dates
        const year = 2025;
        
        try {
          const date = new Date(year, monthNum - 1, dayNum); // Month is 0-indexed in JS
          
          // Validate the date
          if (date.getFullYear() === year && date.getMonth() === monthNum - 1 && date.getDate() === dayNum) {
            dutchDates.push({
              date,
              index: match.index,
              fullMatch: match[0]
            });
            logger.info(`Found Dutch date: ${match[0]} -> ${date.toISOString().split('T')[0]}`);
          }
        } catch (e) {
          logger.warn(`Invalid date: ${match[0]} - ${e}`);
        }
      }
    }
    
    // Also look for DD-MM-YYYY format as backup
    const ddMmYyyyPattern = /(\d{1,2})-(\d{1,2})-(\d{4})/g;
    while ((match = ddMmYyyyPattern.exec(html)) !== null) {
      const [, day, month, year] = match;
      try {
        const date = new Date(parseInt(year), parseInt(month) - 1, parseInt(day));
        dutchDates.push({
          date,
          index: match.index,
          fullMatch: match[0]
        });
        logger.info(`Found DD-MM-YYYY date: ${match[0]} -> ${date.toISOString().split('T')[0]}`);
      } catch (e) {
        logger.warn(`Invalid date: ${match[0]} - ${e}`);
      }
    }
    
    // Find waste type positions
    const wasteTypes = ['restafval', 'gft', 'papier', 'pmd', 'textiel', 'glas'];
    const wastePositions: Array<{type: string, index: number}> = [];
    
    for (const wasteType of wasteTypes) {
      // Look for class attributes
      const classPattern = new RegExp(`class="[^"]*${wasteType}[^"]*"`, 'gi');
      while ((match = classPattern.exec(html)) !== null) {
        wastePositions.push({
          type: wasteType,
          index: match.index
        });
      }
      
      // Also look for text content
      const textPattern = new RegExp(`>${wasteType}<`, 'gi');
      while ((match = textPattern.exec(html)) !== null) {
        wastePositions.push({
          type: wasteType,
          index: match.index
        });
      }
    }
    
    logger.info(`Found ${wastePositions.length} waste type positions`);
    
    // Match dates with nearby waste types
    for (const dateInfo of dutchDates) {
      const nearbyWasteTypes = wastePositions.filter(wt => 
        Math.abs(wt.index - dateInfo.index) <= 2000
      );
      
      if (nearbyWasteTypes.length > 0) {
        // Use the closest waste type
        const closestWasteType = nearbyWasteTypes.reduce((closest, current) => 
          Math.abs(current.index - dateInfo.index) < Math.abs(closest.index - dateInfo.index) ? current : closest
        );
        
        // Check if this event is already added (avoid duplicates)
        const eventKey = `${dateInfo.date.toDateString()}-${this.mapWasteType(closestWasteType.type)}`;
        const isDuplicate = events.some(event => 
          `${event.date.toDateString()}-${event.type}` === eventKey
        );
        
        if (!isDuplicate) {
          events.push({
            type: this.mapWasteType(closestWasteType.type),
            date: dateInfo.date,
            description: `Waste collection: ${closestWasteType.type} on ${dateInfo.fullMatch}`
          });
          logger.info(`Created event: ${closestWasteType.type} on ${dateInfo.fullMatch} -> ${dateInfo.date.toISOString().split('T')[0]}`);
        } else {
          logger.info(`Skipping duplicate: ${closestWasteType.type} on ${dateInfo.fullMatch}`);
        }
      }
    }
    
    // Sort events by date
    events.sort((a, b) => a.date.getTime() - b.date.getTime());
    
    return events;
  }

  /**
   * Filter events to include the whole year starting from current date
   */
  private filterEventsForWholeYear(events: WasteCollectionEvent[]): WasteCollectionEvent[] {
    const now = new Date();
    const startDate = new Date(now.getFullYear(), now.getMonth(), now.getDate()); // Start from today
    const endDate = new Date(now.getFullYear() + 1, now.getMonth(), now.getDate()); // One year from today
    
    logger.info(`Filtering events: now=${now.toDateString()}, startDate=${startDate.toDateString()}, endDate=${endDate.toDateString()}`);
    
    const filteredEvents = events.filter(event => 
      event.date >= startDate && event.date <= endDate
    );
    
    logger.info(`Found ${events.length} total events, filtered to ${filteredEvents.length} events for the whole year`);
    
    return filteredEvents;
  }

  /**
   * Map waste collection types to standardized names
   */
  private mapWasteType(wasteType: string): string {
    const lowerWasteType = wasteType.toLowerCase();
    
    if (lowerWasteType.includes('restafval') || lowerWasteType.includes('grijze') || lowerWasteType.includes('general')) {
      return 'Restafval';
    }
    if (lowerWasteType.includes('gft') || lowerWasteType.includes('groen') || lowerWasteType.includes('green')) {
      return 'GFT';
    }
    if (lowerWasteType.includes('papier') || lowerWasteType.includes('paper')) {
      return 'Papier';
    }
    if (lowerWasteType.includes('pmd') || lowerWasteType.includes('plastic') || lowerWasteType.includes('metaal')) {
      return 'PMD';
    }
    if (lowerWasteType.includes('textiel') || lowerWasteType.includes('textile')) {
      return 'Textiel';
    }
    if (lowerWasteType.includes('glas') || lowerWasteType.includes('glass')) {
      return 'Glas';
    }
    
    return wasteType;
  }
}

